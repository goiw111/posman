#include "posman-panel-list.h"
#include "posman-panel-list-row.h"

#include <sqlite3.h>

struct _PosmanPanelList
{
  GtkStack                  parent_instance;

  GtkWidget                 *cust_listbox;
  GtkWidget                 *cmnd_listbox;
  GtkWidget                 *add_cust;

  GtkWidget                 *listbox_cust;
  GtkWidget                 *listbox_cmnd;

  GtkWidget                 *domain_combobox;
  GtkWidget                 *description_textview;
  GtkWidget                 *name_entry;
  GtkWidget                 *phone_entry;
  GtkWidget                 *adress_entry;

  /* property */

  posmanpanellistview        view;
  posmanpanellistview        previous_view;
  GtkListBoxRow             *edit_row;

  gint                       cust_id;
  gint                       cust_domain;
  GtkListStore              *cmnd_model;
  GtkListStore              *domn_model;
};

G_DEFINE_TYPE (PosmanPanelList, posman_panel_list, GTK_TYPE_STACK)

enum {
  PROP_0,

  VIEW,
  CMND_MODEL,
  DOMN_MODEL,
  CUST_ID,
  CUST_DOMAIN,

  N_PROPS
};

enum {
  ADD_CUST,

  LOAD_CUST,
  REMOVE_CUST,
  PRINT_CUST,
  SHARE_CUST,
  EDIT_CUST,
  LOAD_EDIT_CUST,

  ADD_CMND,
  LOAD_CMND,
  REMOVE_CMND,
  LOAD_PAY,
  PRINT_CMND,
  SHAR_CMND,

  LAST_SIGNAL
};

static GParamSpec *properties [N_PROPS] = { NULL, };
static guint       signals[LAST_SIGNAL];

/*
 * Auxiliary methods
 */

static GtkWidget*
get_listbox_from_view(PosmanPanelList     *self,
                      posmanpanellistview view)
{
  switch (view)
    {
    case posman_panel_list_main:
      return self->cust_listbox;

    case posman_panel_list_cust:
      return self->cmnd_listbox;

    case posman_panel_list_add_cust:
      return self->add_cust;

    default:
      return NULL;
    }
}

static void
add_cust_clear_data(PosmanPanelList   *self)
{
  gtk_entry_set_text (GTK_ENTRY(self->name_entry),"");
  gtk_entry_set_text (GTK_ENTRY(self->adress_entry),"");
  gtk_entry_set_text (GTK_ENTRY(self->phone_entry),"");
  gtk_text_buffer_set_text(gtk_text_view_get_buffer (GTK_TEXT_VIEW (self->description_textview)),
                           "",
                           -1);
  self->edit_row = NULL;

}

/*callback*/


static void
add_cmnd_cb(GtkButton *button,
            gpointer   user_data)
{
  PosmanPanelList *self = (PosmanPanelList *)user_data;

  g_signal_emit (self, signals[ADD_CMND],0);

}
static void
add_cust_cb(GtkButton *button,
            gpointer   user_data)
{
  PosmanPanelList *self = (PosmanPanelList *)user_data;
  posman_panel_list_set_view(self,posman_panel_list_add_cust);
  add_cust_clear_data(self);
}

static void
load_cust_cb(GtkButton *button,
             gpointer   user_data)
{
}

static void
remove_cust_cb(GtkButton *button,
               gpointer   user_data)
{
  PosmanPanelList *self = (PosmanPanelList *)user_data;
  GtkListBoxRow   *row;

  row = gtk_list_box_get_selected_row(GTK_LIST_BOX (self->listbox_cust));
  if(row)
  g_signal_emit (self, signals[REMOVE_CUST],0,row,self->listbox_cust);
  else
    g_message ("sorry you should select a row to remove it");
}

static void
print_cust_cb(GtkButton *button,
              gpointer   user_data)
{
}

static void
listbox_cust_load(PosmanPanelList  *self,
                  GtkListBoxRow    *row)
{
  CustRowData     *cust_data = g_object_get_data(G_OBJECT(row),"data");

  self->cust_domain = cust_data->cust_domain;

  g_signal_emit(self,signals[LOAD_CUST],0,row);
}

static void
listbox_cust_row_activated_cb(GtkListBox    *box,
                              GtkListBoxRow *row,
                              gpointer       user_data)
{
  PosmanPanelList *self = (PosmanPanelList *)user_data;

  listbox_cust_load(self,row);
}

static void
share_cust_cb(GtkButton *button,
              gpointer   user_data)
{
}

static void
edit_cust_cb(GtkButton *button,
             gpointer   user_data)
{
  PosmanPanelList *self = (PosmanPanelList *)user_data;
  GtkListBoxRow   *row;

  row = gtk_list_box_get_selected_row(GTK_LIST_BOX (self->listbox_cust));
  if(row)
    {
      posman_panel_list_set_view (self, posman_panel_list_add_cust);
      add_cust_clear_data(self);
      g_signal_emit (self,signals[LOAD_EDIT_CUST],0,row );
    }
  else
    g_message ("sorry you should select a row to edit it");
}

void
load_cmnd_cb0(GtkButton *button,
              gpointer   user_data)
{
  PosmanPanelList *self = (PosmanPanelList *)user_data;
  GtkListBoxRow   *row;

  row = gtk_list_box_get_selected_row(GTK_LIST_BOX (self->listbox_cmnd));
  if(row)
    {
      g_signal_emit (self,signals[LOAD_CMND],0);
    }
  else
    g_message ("sorry you should select a row to edit it");

}

void
remove_cmnd_cb(GtkButton *button,
               gpointer   user_data)
{
  PosmanPanelList *self = (PosmanPanelList *)user_data;
  GtkListBoxRow   *row;

  row = gtk_list_box_get_selected_row(GTK_LIST_BOX (self->listbox_cmnd));
  if(row)
    {
      g_signal_emit (self,signals[REMOVE_CMND],0,row);
    }
  else
    g_message ("sorry you should select a row to remove it");
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

  if(self->cmnd_model)g_object_unref ( self->cmnd_model);

  G_OBJECT_CLASS (posman_panel_list_parent_class)->dispose(object);
}


static void
posman_panel_list_constracted(GObject *object)
{
  PosmanPanelList *self       = POSMAN_PANEL_LIST (object);

  G_OBJECT_CLASS (posman_panel_list_parent_class)->constructed(object);

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
      case VIEW:
        g_value_set_int (value, self->view);
        break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
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
      case VIEW:
        posman_panel_list_set_view (self, g_value_get_int (value));
        break;
      case CUST_DOMAIN:
        posman_panel_list_set_cust_domain (self, g_value_get_int (value));
        break;
      case CUST_ID:
        posman_panel_list_set_cust_id (self, g_value_get_int (value));
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

  gtk_widget_class_bind_template_child (widget_class,PosmanPanelList,cust_listbox);
  gtk_widget_class_bind_template_child (widget_class,PosmanPanelList,cmnd_listbox);
  gtk_widget_class_bind_template_child (widget_class,PosmanPanelList,add_cust);
  gtk_widget_class_bind_template_child (widget_class,PosmanPanelList,listbox_cust);
  gtk_widget_class_bind_template_child (widget_class,PosmanPanelList,listbox_cmnd);
  gtk_widget_class_bind_template_child (widget_class,PosmanPanelList,domain_combobox);
  gtk_widget_class_bind_template_child (widget_class,PosmanPanelList,description_textview);
  gtk_widget_class_bind_template_child (widget_class,PosmanPanelList,name_entry);
  gtk_widget_class_bind_template_child (widget_class,PosmanPanelList,phone_entry);
  gtk_widget_class_bind_template_child (widget_class,PosmanPanelList,adress_entry);

  gtk_widget_class_bind_template_callback (widget_class, add_cust_cb);
  gtk_widget_class_bind_template_callback (widget_class, load_cust_cb);
  gtk_widget_class_bind_template_callback (widget_class, remove_cust_cb);
  gtk_widget_class_bind_template_callback (widget_class, edit_cust_cb);
  gtk_widget_class_bind_template_callback (widget_class, share_cust_cb);
  gtk_widget_class_bind_template_callback (widget_class, print_cust_cb);
  gtk_widget_class_bind_template_callback (widget_class, listbox_cust_row_activated_cb);
  gtk_widget_class_bind_template_callback (widget_class, add_cmnd_cb);
  gtk_widget_class_bind_template_callback (widget_class, load_cmnd_cb0);
  gtk_widget_class_bind_template_callback (widget_class, remove_cmnd_cb);

  object_class->finalize = posman_panel_list_finalize;
  object_class->dispose  = posman_panel_list_dispose;
  object_class->constructed = posman_panel_list_constracted;
  object_class->get_property = posman_panel_list_get_property;
  object_class->set_property = posman_panel_list_set_property;

  properties[VIEW] =
  g_param_spec_int ("view",
                    "panel list view",
                    "the view of the panel list",
                    posman_panel_list_main,
                    posman_panel_list_add_cust,
                    posman_panel_list_main,
                    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_CONSTRUCT);

  properties[CUST_ID] =
  g_param_spec_int ("cust-id",
                    "customer id",
                    "the current customer id in the cmnd view",
                    -1,
                    G_MAXINT,
                    -1,
                    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_CONSTRUCT);

  properties[CUST_DOMAIN] =
  g_param_spec_int ("cust-domain",
                    "customer domain",
                    "the current customer domain in the cmnd view",
                    -1,
                    G_MAXINT,
                    -1,
                    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_CONSTRUCT);


  properties[CMND_MODEL] =
  g_param_spec_object("cmnd-model",
                      "commond model",
                      "the model that contain the commond",
                      GTK_TYPE_TREE_MODEL,
                      G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY );

  properties[DOMN_MODEL] =
  g_param_spec_object("domn-model",
                      "domain model",
                      "the model that contain the domains",
                      GTK_TYPE_TREE_MODEL,
                      G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY );

  g_object_class_install_properties (object_class, N_PROPS, properties);

  signals[ADD_CUST] = g_signal_new("add-cust",
                                   G_OBJECT_CLASS_TYPE (object_class),
                                   G_SIGNAL_RUN_FIRST,
                                   0,
                                   NULL,NULL,
                                   NULL,
                                   G_TYPE_NONE,
                                   5,G_TYPE_STRING,
                                   G_TYPE_STRING,
                                   G_TYPE_STRING,
                                   G_TYPE_STRING,
                                   G_TYPE_STRING);
  signals[LOAD_CUST] = g_signal_new("load-cust",
                                    G_OBJECT_CLASS_TYPE (object_class),
                                    G_SIGNAL_RUN_FIRST,0,
                                    NULL,NULL,
                                    NULL,
                                    G_TYPE_NONE,1,
                                    GTK_TYPE_LIST_BOX_ROW);
  signals[PRINT_CUST] = g_signal_new("print-cust",
                                   G_OBJECT_CLASS_TYPE (object_class),
                                   G_SIGNAL_RUN_FIRST,
                                   0,
                                   NULL,NULL,
                                   NULL,
                                   G_TYPE_NONE,
                                   0);
  signals[SHARE_CUST] = g_signal_new("share-cust",
                                   G_OBJECT_CLASS_TYPE (object_class),
                                   G_SIGNAL_RUN_FIRST,
                                   0,
                                   NULL,NULL,
                                   NULL,
                                   G_TYPE_NONE,
                                   0);
  signals[EDIT_CUST] = g_signal_new("edit-cust",
                                    G_OBJECT_CLASS_TYPE (object_class),
                                    G_SIGNAL_RUN_FIRST,
                                    0,
                                    NULL,NULL,
                                    NULL,
                                    G_TYPE_NONE,
                                    6,GTK_TYPE_LIST_BOX_ROW,
                                    G_TYPE_STRING,
                                    G_TYPE_STRING,
                                    G_TYPE_STRING,
                                    G_TYPE_STRING,
                                    G_TYPE_STRING);
  signals[LOAD_EDIT_CUST] = g_signal_new("load-edit-cust",
                                    G_OBJECT_CLASS_TYPE (object_class),
                                    G_SIGNAL_RUN_FIRST,
                                    0,
                                    NULL,NULL,
                                    NULL,
                                    G_TYPE_NONE,
                                    1,GTK_TYPE_LIST_BOX_ROW);
  signals[ADD_CMND] = g_signal_new("add-cmnd",
                                   G_OBJECT_CLASS_TYPE (object_class),
                                   G_SIGNAL_RUN_FIRST,
                                   0,
                                   NULL,NULL,
                                   NULL,
                                   G_TYPE_NONE,
                                   0);
  signals[LOAD_CMND] = g_signal_new("load-cmnd",
                                   G_OBJECT_CLASS_TYPE (object_class),
                                   G_SIGNAL_RUN_FIRST,
                                   0,
                                   NULL,NULL,
                                   NULL,
                                   G_TYPE_NONE,
                                   0);
  signals[REMOVE_CMND] = g_signal_new("remove-cmnd",
                                   G_OBJECT_CLASS_TYPE (object_class),
                                   G_SIGNAL_RUN_FIRST,
                                   0,
                                   NULL,NULL,
                                   NULL,
                                   G_TYPE_NONE,
                                   1,GTK_TYPE_LIST_BOX_ROW);
  signals[LOAD_PAY] = g_signal_new("load-pay",
                                   G_OBJECT_CLASS_TYPE (object_class),
                                   G_SIGNAL_RUN_FIRST,
                                   0,
                                   NULL,NULL,
                                   NULL,
                                   G_TYPE_NONE,
                                   0);
  signals[PRINT_CMND] = g_signal_new("print-cmnd",
                                   G_OBJECT_CLASS_TYPE (object_class),
                                   G_SIGNAL_RUN_FIRST,
                                   0,
                                   NULL,NULL,
                                   NULL,
                                   G_TYPE_NONE,
                                   0);
  signals[SHAR_CMND] = g_signal_new("shar-cmnd",
                                   G_OBJECT_CLASS_TYPE (object_class),
                                   G_SIGNAL_RUN_FIRST,
                                   0,
                                   NULL,NULL,
                                   NULL,
                                   G_TYPE_NONE,
                                   0);
  signals[REMOVE_CUST] = g_signal_new("remove-cust",
                                      G_OBJECT_CLASS_TYPE (object_class),
                                      G_SIGNAL_RUN_FIRST,
                                      0,
                                      NULL,NULL,
                                      NULL,
                                      G_TYPE_NONE,
                                      2,GTK_TYPE_LIST_BOX,
                                      GTK_TYPE_LIST_BOX_ROW);

}

static void
posman_panel_list_init (PosmanPanelList *self)
{

  self->cmnd_model = gtk_list_store_new (2,G_TYPE_INT,G_TYPE_POINTER);
  self->domn_model = gtk_list_store_new (2,G_TYPE_STRING,G_TYPE_STRING);


  gtk_widget_init_template (GTK_WIDGET (self));

  gtk_combo_box_set_model (GTK_COMBO_BOX (self->domain_combobox),
                           GTK_TREE_MODEL ( self->domn_model));
}

void
posman_panel_list_set_view(PosmanPanelList    *self,
                           posmanpanellistview view)
{
  g_return_if_fail (POSMAN_IS_PANEL_LIST (self));

  if(self->view == view)
    return;

  gboolean crossfade = view == posman_panel_list_add_cust || self->view == posman_panel_list_add_cust;

  self->view = view;

  gtk_stack_set_transition_type(GTK_STACK (self),
                                crossfade ? GTK_STACK_TRANSITION_TYPE_CROSSFADE
                                : GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);

  gtk_stack_set_visible_child (GTK_STACK (self),
                               get_listbox_from_view (self, view));

  g_object_notify_by_pspec (G_OBJECT (self),properties[VIEW]);
}

void
posman_panel_list_set_cust_id(PosmanPanelList    *self,
                              int                 cust_id)
{
  g_return_if_fail (POSMAN_IS_PANEL_LIST (self));

  self->cust_id = cust_id;

  g_object_notify_by_pspec (G_OBJECT (self),properties[CUST_ID]);
}

int
posman_panel_list_get_cust_id(PosmanPanelList  *self)
{
  g_return_val_if_fail (POSMAN_IS_PANEL_LIST (self),-1);
  return self->cust_id;
}

posmanpanellistview
posman_panel_list_get_view(PosmanPanelList  *self)
{
  g_return_val_if_fail (POSMAN_IS_PANEL_LIST (self),-1);
  return self->view;
}

/* should remove */

GtkListStore *
posman_panel_list_get_domn_model(PosmanPanelList  *self)
{
  g_return_val_if_fail (POSMAN_IS_PANEL_LIST (self),NULL);
  return  self->domn_model;
}

gboolean
posman_panel_list_check_data(PosmanPanelList  *self,
                             GtkWidget        *content_label)
{
  g_return_val_if_fail (POSMAN_IS_PANEL_LIST (self),FALSE);

  gchar                 *query;
  g_autofree gchar      *name = NULL;
  g_autofree gchar      *phone = NULL;
  g_autofree gchar      *address = NULL;
  g_autofree gchar      *domain = NULL;
  g_autofree gchar      *description = NULL;
  GtkTextIter            start, end;
  GtkTextBuffer         *buffer;


  buffer      = gtk_text_view_get_buffer (GTK_TEXT_VIEW (self->description_textview));
  gtk_text_buffer_get_bounds (buffer, &start, &end);

  name        = g_strdup ((gtk_entry_get_text (GTK_ENTRY (self->name_entry))));
  phone       = g_strdup (gtk_entry_get_text (GTK_ENTRY (self->phone_entry)));
  address     = g_strdup (gtk_entry_get_text (GTK_ENTRY (self->adress_entry)));
  domain      = g_strdup (gtk_combo_box_get_active_id (GTK_COMBO_BOX (self->domain_combobox)));
  description = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

  query = g_strdup_printf("%s%s%s%s",
                          strlen(name) > 5 ? "" : " the name not acceptable",
                          (strlen(phone) == 0 ||
                           strlen(phone) >= 10) ? "" : " the number not acceptable",
                          strlen(address) > 3 ? "" : " the address not acceptable",
                          domain != NULL ? "" : " please select a domain");

  if(strlen (query) != 0)
    {
      gtk_label_set_text (GTK_LABEL(content_label), query);
      free(query);
      return FALSE;
    }
  free(query);

  if(self->edit_row)
    g_signal_emit (self, signals[EDIT_CUST], 0,
                 self->edit_row,name,address,domain,description,phone);
  else
  g_signal_emit (self, signals[ADD_CUST], 0,
                 name,address,phone,domain,description);
  return TRUE;
}

void
posman_panel_list_insert_cust(PosmanPanelList   *self,
                              GtkWidget         *cust_row)
{

  gtk_list_box_insert(GTK_LIST_BOX (self->listbox_cust),
                      GTK_WIDGET (cust_row),-1);
}

void
posman_panel_list_insert_cmnd(PosmanPanelList   *self,
                              GtkWidget         *cmnd_row)
{
  gtk_list_box_insert(GTK_LIST_BOX (self->listbox_cmnd),
                      GTK_WIDGET (cmnd_row),-1);
}

void
posman_panel_list_remove_cmnd(PosmanPanelList   *self,
                              GtkWidget         *cmnd_row)
{
  gtk_container_remove(GTK_CONTAINER (self->listbox_cmnd),
                      GTK_WIDGET (cmnd_row));
}

void
posman_panel_list_foreach(GtkWidget *widget,
                          gpointer data)
{
  gtk_container_remove(GTK_CONTAINER (data),widget);
}

void
posman_panel_list_clear(PosmanPanelList *self)
{
  gtk_container_foreach(GTK_CONTAINER (self->listbox_cmnd),
                        posman_panel_list_foreach,
                        self->listbox_cmnd);
}

void
posman_panel_list_load_edit_cust(PosmanPanelList   *self,
                                 GtkListBoxRow     *row,
                                 gchar             *name,
                                 gchar             *address,
                                 gchar             *domain_id,
                                 gchar             *description,
                                 gchar             *phone)
{
  GtkTextBuffer   *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (self->description_textview));

  gtk_entry_set_text (GTK_ENTRY(self->name_entry),name == NULL?"":name);
  gtk_entry_set_text (GTK_ENTRY(self->adress_entry),address == NULL?"":address);
  gtk_entry_set_text (GTK_ENTRY(self->phone_entry),phone == NULL?"":phone);
  gtk_text_buffer_insert_at_cursor(buffer,description,-1);
  gtk_combo_box_set_active_id(GTK_COMBO_BOX (self->domain_combobox),domain_id);

  self->edit_row = row;
}

GtkListBoxRow  *
posman_panel_list_get_selected_cust(PosmanPanelList   *self)
{
  return gtk_list_box_get_selected_row(GTK_LIST_BOX (self->listbox_cust));
}

void
posman_panel_list_select_cmnd(PosmanPanelList   *self,GtkListBoxRow   *row)
{
  gtk_list_box_select_row(GTK_LIST_BOX(self->listbox_cmnd),row);
}

GtkListBoxRow  *
posman_panel_list_get_selected_cmnd(PosmanPanelList   *self)
{
  return gtk_list_box_get_selected_row(GTK_LIST_BOX (self->listbox_cmnd));
}

gint
posman_panel_list_get_cust_domain(PosmanPanelList   *self)
{
  g_return_val_if_fail (POSMAN_IS_PANEL_LIST (self),-1);

  return self->cust_domain;
}

void
posman_panel_list_set_cust_domain(PosmanPanelList   *self,gint    cust_domain)
{
  g_return_if_fail (POSMAN_IS_PANEL_LIST (self));

  self->cust_domain = cust_domain;

  g_object_notify_by_pspec (G_OBJECT (self),properties[CUST_DOMAIN]);
}

GtkWidget *
posman_panel_list_get_cmnd_row(PosmanPanelList   *self,gint   cmnd_id)
{
  g_return_val_if_fail (POSMAN_IS_PANEL_LIST (self),NULL);

  GList   *list = gtk_container_get_children(GTK_CONTAINER (self->listbox_cmnd));

  GList *l = list;
  while (l != NULL)
    {
      CmndRowData *cmnd = g_object_get_data (G_OBJECT (l->data),"data");
      if (cmnd->cmnd_id == cmnd_id)
        return  l->data;
      l = l->next;
    }

  return NULL;
}

void
posman_panel_list_reload_cust(PosmanPanelList   *self)
{
  listbox_cust_load(self,
                    gtk_list_box_get_selected_row(GTK_LIST_BOX (self->listbox_cust)));
}

static void
gtk_widget_destroy_cb(GtkWidget   *widget,
                      gpointer     user_data)
{
  gtk_container_remove(GTK_CONTAINER (user_data),
                       widget);
}

void
posman_panel_list_clear_cust(PosmanPanelList   *self)
{
  gtk_container_foreach(GTK_CONTAINER (self->cust_listbox),
                        gtk_widget_destroy_cb,
                        self->cust_listbox);
}
