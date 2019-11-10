#include "posman-panel-list.h"
#include "posman-panel-model.h"
#include "posman-panel-loader.h"

struct _PosmanPanelList
{
  GtkStack    parent_instance;

  GtkWidget                 *main_listbox;
  GtkWidget                 *cust_listbox;

  posmanpanellistview       view;
  gint                      cust_id;

  PosmanPanelModel          *cust_model;
};
typedef struct {
  GtkWidget                 *row;

  gchar                     *id;
  gchar                     *name;
}RowMainData;

typedef struct {
  GtkWidget                 *row;

  gchar                     *commond_id;
  gchar                     *full_name;
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
posman_panel_list_add_panel(PosmanPanelList *self, const gchar *id, const gchar *name)
{
  RowMainData   *row;

  g_return_if_fail (POSMAN_IS_PANEL_LIST (self));

  row = row_main_data_new(id,name);
  gtk_container_add (GTK_CONTAINER (self->main_listbox), row->row);

}

static void
posman_panel_list_load_panels(PosmanPanelList *self)
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
                          COL_ID,&id,
                          COL_NAME,&name,
                          -1);

      posman_panel_list_add_panel (self, id, name);

      valid = gtk_tree_model_iter_next (GTK_TREE_MODEL (self->cust_model),
                                        &iter);
    }
}

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
postman_panel_list_set_view(PosmanPanelList *self,
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
        postman_panel_list_set_view (self, g_value_get_int (value));
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

  object_class->finalize = posman_panel_list_finalize;
  object_class->get_property = posman_panel_list_get_property;
  object_class->set_property = posman_panel_list_set_property;

  properties[PROP_VIEW] =
  g_param_spec_int ("view",
                    "view","the current view of the sidelist",
                    posman_panel_list_main ,
                    posman_panel_list_cust ,
                    posman_panel_list_main ,
                    G_PARAM_READWRITE );
  g_object_class_install_properties (object_class, N_PROPS, properties);

}

static void
posman_panel_list_init (PosmanPanelList *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
  self->cust_model = g_object_new (POSMAN_TYPE_PANEL_MODEL, "db-init",TRUE,NULL);
  posman_panel_loader_get_cust(POSMAN_PANEL_LOADER (self->cust_model));
  posman_panel_list_load_panels(self);


}
