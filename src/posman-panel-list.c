#include "posman-panel-list.h"
#include "posman-panel-model.h"
#include "posman-panel-loader.h"
#include <sqlite3.h>

struct _PosmanPanelList
{
  GtkStack                  parent_instance;

  GtkWidget                 *main_listbox;

  GtkWidget                 *cust_listbox;
  gchar                     *cust_id;

  posmanpanellistview       view;

  PosmanPanelModel          *cust_model;
  PosmanPanelModel          *cmnd_model;

  sqlite3                   *db;
};


typedef struct {
  GtkWidget                 *row;

  gchar                     *id;
  gchar                     *name;

}RowMainData;


typedef struct {
  GtkWidget                 *row;

  gchar                     *id;
  gchar                     *state;
  gchar                     *date;
  gchar                     *total;

}RowCustData;



G_DEFINE_TYPE (PosmanPanelList, posman_panel_list, GTK_TYPE_STACK)

enum {
  PROP_0,
  PROP_VIEW,
  N_PROPS
};

static GParamSpec *properties [N_PROPS] = { NULL, };

/*
 * RowData functions
 */

static void
row_main_data_free(RowMainData *row)
{
  free(row->id);
  free(row->name);
  free(row);
}

static RowMainData *
row_main_data_new(const gchar *id, const gchar *name)
{
  RowMainData *row    = g_new0 (RowMainData, 1);
  GtkWidget   *grid,*label;
  row->row  = gtk_list_box_row_new ();
  row->id   = g_strdup (id);
  row->name = g_strdup (name);

  grid = g_object_new (GTK_TYPE_GRID,
                       "visible", TRUE,
                       "hexpand", TRUE,
                       "border-width", 12,
                       "column-spacing", 12,
                       NULL);

  label = g_object_new (GTK_TYPE_LABEL,
                        "label", name,
                        "visible", TRUE,
                        "xalign", 0.0,
                        "hexpand", TRUE,
                        NULL);

  gtk_grid_attach (GTK_GRID (grid), label, 0, 0, 1, 1);
  gtk_container_add (GTK_CONTAINER (row->row), grid);
  gtk_widget_show (row->row);

  g_object_set_data_full (G_OBJECT (row->row), "data", row, (GDestroyNotify) row_main_data_free);

  return row;
}

static void
row_cust_data_free(RowCustData *row)
{
  free(row->id);
  free(row->state);
  free(row->date);
  free(row->total);
  free(row);
}

static RowCustData *
row_cust_data_new(const gchar *id,
                  const gchar *state,
                  const gchar *date,
                  const gchar *total)
{
  RowCustData *row    = g_new0 (RowCustData, 1);
  GtkWidget   *grid,*label;
  row->row  = gtk_list_box_row_new ();
  row->id   = g_strdup (id);
  row->state = g_strdup (state);
  row->date = g_strdup (date);
  row->total = g_strdup (total);

  grid = g_object_new (GTK_TYPE_GRID,
                       "visible", TRUE,
                       "hexpand", TRUE,
                       "border-width", 12,
                       "column-spacing", 12,
                       NULL);

  label = g_object_new (GTK_TYPE_LABEL,
                        "label", date,
                        "visible", TRUE,
                        "xalign", 0.0,
                        "hexpand", TRUE,
                        NULL);

  gtk_grid_attach (GTK_GRID (grid), label, 0, 0, 1, 1);
  gtk_container_add (GTK_CONTAINER (row->row), grid);
  gtk_widget_show (row->row);

  g_object_set_data_full (G_OBJECT (row->row), "data", row, (GDestroyNotify) row_cust_data_free);

  return row;
}

/*
 * Auxiliary methods
 */
posmanpanellistview
posman_panel_list_get_view(PosmanPanelList *self)
{
  g_return_val_if_fail (POSMAN_IS_PANEL_LIST (self), -1);
  return self->view;
}

static GtkWidget*
get_listbox_from_view(PosmanPanelList     *self,
                      posmanpanellistview view)
{
  switch (view)
    {
    case posman_panel_list_main:
      return self->main_listbox;

    case posman_panel_list_cust:
      return self->cust_listbox;

    default:
      return NULL;
    }
}

static void
posman_panel_list_add_panel(PosmanPanelList *self,
                            const gchar *id,
                            const gchar *name)
{
  RowMainData   *row;

  g_return_if_fail (POSMAN_IS_PANEL_LIST (self));

  row = row_main_data_new(id,name);
  gtk_container_add (GTK_CONTAINER (self->main_listbox), row->row);

}

static void
posman_panel_list_add_cmnd(PosmanPanelList *self,
                           const gchar *id,
                           const gchar *state,
                           const gchar *date,
                           const gchar *total)
{
  RowCustData   *row;

  g_return_if_fail (POSMAN_IS_PANEL_LIST (self));

  row = row_cust_data_new(id,state,date,total);
  gtk_container_add (GTK_CONTAINER (self->cust_listbox), row->row);

}


/*callback*/

static void
posman_panel_list_cust_listbox_remove_all(GtkListBox   *box)
{
  GList   *iter;

  iter = gtk_container_get_children(GTK_CONTAINER (box));
  while (iter != NULL)
    {
      gtk_container_remove(GTK_CONTAINER (box),GTK_WIDGET(iter->data));
      iter = g_list_next(iter);
    }
  g_list_free(iter);

}

static void
main_row_activated_cb(GtkListBox      *box,
                      GtkListBoxRow   *row,
                      PosmanPanelList *self)
{
  RowMainData *data = g_object_get_data(G_OBJECT(row),"data");

  if(!g_strcmp0(data->id,self->cust_id))
    {
      posman_panel_list_set_view(self,posman_panel_list_cust);
      return;
    }

  self->cust_id = data->id;
  posman_panel_list_cust_listbox_remove_all(GTK_LIST_BOX (self->cust_listbox));

  posman_panel_list_cust_model_init(self,self->db,data->id);
  posman_panel_list_cust_load_panels(POSMAN_PANEL_LIST (self));
  posman_panel_list_set_view(self,posman_panel_list_cust);
}

static void
cust_row_activated_cb(GtkListBox      *box,
                      GtkListBoxRow   *row,
                      PosmanPanelList *self)
{

}

/*object*/

PosmanPanelList *
posman_panel_list_new (void)
{
  return g_object_new (POSMAN_TYPE_PANEL_LIST, NULL);
}

static void
posman_panel_list_finalize (GObject *object)
{
  PosmanPanelList *self = (PosmanPanelList *)object;

  G_OBJECT_CLASS (posman_panel_list_parent_class)->finalize (object);
}

static void
posman_panel_list_dispose(GObject *object)
{
  PosmanPanelList *self = (PosmanPanelList *)object;

  g_clear_object (&self->cust_model);
  g_clear_object (&self->cmnd_model);

  G_OBJECT_CLASS (posman_panel_list_parent_class)->dispose(object);
}

static void
posman_panel_list_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  PosmanPanelList *self = POSMAN_PANEL_LIST (object);

  switch (prop_id)
    {
      case PROP_VIEW:
        g_value_set_int (value, self->view);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

void
posman_panel_list_set_view(PosmanPanelList *self,
                            posmanpanellistview view)
{
  g_return_if_fail (POSMAN_IS_PANEL_LIST (self));

  if(self->view == view)
    return;
  self->view = view;

  gtk_stack_set_visible_child (GTK_STACK (self),
                               get_listbox_from_view (self, view));

  g_object_notify_by_pspec (G_OBJECT (self),
                            properties[PROP_VIEW]);

}

static void
posman_panel_list_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  PosmanPanelList *self = POSMAN_PANEL_LIST (object);

  switch (prop_id)
    {
      case PROP_VIEW:
        posman_panel_list_set_view (self, g_value_get_int (value));
        break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
posman_panel_list_class_init (PosmanPanelListClass *klass)
{
  GObjectClass    *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(klass);

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/pos/manager/posman-panel-list.ui");

  gtk_widget_class_bind_template_child (widget_class,
                                        PosmanPanelList,
                                        main_listbox);
  gtk_widget_class_bind_template_child (widget_class,
                                        PosmanPanelList,
                                        cust_listbox);

  gtk_widget_class_bind_template_callback (widget_class,
                                           main_row_activated_cb);
  gtk_widget_class_bind_template_callback (widget_class,
                                           cust_row_activated_cb);

  object_class->finalize = posman_panel_list_finalize;
  object_class->dispose  = posman_panel_list_dispose;
  object_class->get_property = posman_panel_list_get_property;
  object_class->set_property = posman_panel_list_set_property;

  properties[PROP_VIEW] =
  g_param_spec_int ("view",
                    "view",
                    "the current view of the sidelist",
                    posman_panel_list_main ,
                    posman_panel_list_cust ,
                    posman_panel_list_main ,
                    G_PARAM_READWRITE | G_PARAM_CONSTRUCT);

  g_object_class_install_properties (object_class, N_PROPS, properties);

}

static void
posman_panel_list_init (PosmanPanelList *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

void
posman_panel_list_main_model_init(PosmanPanelList *self, sqlite3 *db)
{
  PosmanPanelModel *model;

  if(self->db == NULL) self->db = db;

  model = g_object_new (POSMAN_TYPE_PANEL_MODEL,
                        NULL);

  posman_panel_loader_get_cust(POSMAN_PANEL_LOADER (model),db);

  self->cust_model = model;
}

void
posman_panel_list_cust_model_init(PosmanPanelList *self,
                                  sqlite3 *db,
                                  gchar *cust_id)
{
  PosmanPanelModel *model;

  model = g_object_new (POSMAN_TYPE_PANEL_MODEL,
                        "type",posmanpanelmodelcust,
                        NULL);

  posman_panel_loader_get_cmnd(POSMAN_PANEL_LOADER (model),db,cust_id);

  self->cmnd_model = model;
}

void
posman_panel_list_main_load_panels(PosmanPanelList *self)
{
  gboolean    valid;
  GtkTreeIter iter;

  g_assert(self->cust_model != NULL);

  valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (self->cust_model),
                                         &iter);
  while (valid)
    {
      g_autofree gchar *id = NULL;
      g_autofree gchar *name = NULL;

      gtk_tree_model_get (GTK_TREE_MODEL (self->cust_model), &iter,
                          COL_MAIN_ID,&id,
                          COL_MAIN_NAME,&name,
                          -1);

      posman_panel_list_add_panel (self, id, name);

      valid = gtk_tree_model_iter_next (GTK_TREE_MODEL (self->cust_model),
                                        &iter);
    }
}

void
posman_panel_list_cust_load_panels(PosmanPanelList *self)
{
  gboolean    valid;
  GtkTreeIter iter;

  g_assert(self->cust_model != NULL);

  valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (self->cmnd_model),
                                         &iter);
  while (valid)
    {
      g_autofree gchar *id = NULL;
      g_autofree gchar *state = NULL;
      g_autofree gchar *date = NULL;
      g_autofree gchar *total = NULL;

      gtk_tree_model_get (GTK_TREE_MODEL (self->cmnd_model), &iter,
                          COL_CUST_ID,&id,
                          COL_CUST_STATE,&state,
                          COL_CUST_DATE,&date,
                          COL_CUST_TOTAL,&total,
                          -1);

      posman_panel_list_add_cmnd (self, id, state,date,total);

      valid = gtk_tree_model_iter_next (GTK_TREE_MODEL (self->cmnd_model),
                                        &iter);
    }
}
