#include "posman-panel-list.h"
#include "posman-action-menu-cust.h"
#include "posman-action-menu-cmnd.h"
#include "posman-panel-row-cust.h"
#include <sqlite3.h>

struct _PosmanPanelList
{
  GtkStack                  parent_instance;

  GtkWidget                 *cust_listbox;
  GtkWidget                 *cmnd_listbox;
  GtkWidget                 *add_cust;

  posmanpanellistview       view;

  GObject                   *list_stor_cust;
  GObject                   *list_stor_cmnd;
  GtkWidget                 *list_stor_domain;

  GtkWidget                 *menu_cust;
  GtkWidget                 *menu_cmnd;
  GtkWidget                 *slct_add_cust;

  GtkWidget                 *name_entry;
  GtkWidget                 *adress_entry;
  GtkWidget                 *phone_entry;
  GtkWidget                 *domain_combobox;
  GtkWidget                 *description_textview;


};

G_DEFINE_TYPE (PosmanPanelList, posman_panel_list, GTK_TYPE_STACK)

enum {
  PROP_0,
  PROP_VIEW,
  PROP_STOR_CUST,
  PROP_STOR_CMND,
  PROP_STOR_DOMAIN,
  N_PROPS
};
static GParamSpec *properties [N_PROPS] = { NULL, };

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




/*callback*/


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

  if (self->list_stor_cust)
    g_object_unref(self->list_stor_cust);
  if (self->list_stor_cmnd)
    g_object_unref(self->list_stor_cmnd);
  if (self->list_stor_domain)
    g_object_unref(self->list_stor_domain);

  G_OBJECT_CLASS (posman_panel_list_parent_class)->dispose(object);
}

static void
posman_panel_list_constracted(GObject *object)
{
  PosmanPanelList *self = (PosmanPanelList*)object;

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
      case PROP_VIEW:
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
      case PROP_VIEW:
        posman_panel_list_set_view (self, g_value_get_int (value));
      break;
      case PROP_STOR_DOMAIN:
        /*posman_panel_list_set_model_domain(self,g_value_get_object(value));*/
      break;
      case PROP_STOR_CUST:
        posman_panel_list_set_list_stor_cust(self,g_value_get_object(value));
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

  gtk_widget_class_bind_template_child (widget_class,PosmanPanelList,
                                        cust_listbox);
  gtk_widget_class_bind_template_child (widget_class,PosmanPanelList,
                                        cmnd_listbox);
  gtk_widget_class_bind_template_child (widget_class,PosmanPanelList,
                                        add_cust);

  gtk_widget_class_bind_template_child (widget_class,PosmanPanelList,
                                        domain_combobox);
  gtk_widget_class_bind_template_child (widget_class,PosmanPanelList,
                                        name_entry);
  gtk_widget_class_bind_template_child (widget_class,PosmanPanelList,
                                        adress_entry);
  gtk_widget_class_bind_template_child (widget_class,PosmanPanelList,
                                        phone_entry);
  gtk_widget_class_bind_template_child (widget_class,PosmanPanelList,
                                        description_textview);

  object_class->finalize = posman_panel_list_finalize;
  object_class->dispose  = posman_panel_list_dispose;
  object_class->constructed = posman_panel_list_constracted;
  object_class->get_property = posman_panel_list_get_property;
  object_class->set_property = posman_panel_list_set_property;

  properties[PROP_VIEW] =
  g_param_spec_int ("view",
                    "view",
                    "the current view of the sidelist",
                    posman_panel_list_main ,
                    posman_panel_list_add_cust ,
                    posman_panel_list_main ,
                    G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_STOR_CUST] =
  g_param_spec_object("list-stor-cust",
                      "list-stor-cust",
                      "GListStor proprety for customers",
                      G_TYPE_LIST_MODEL,
                      G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_STOR_CMND] =
  g_param_spec_object("list-stor-cmnd",
                      "list-stor-cmnd",
                      "GListStor proprety for commend",
                      G_TYPE_LIST_MODEL,
                      G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_STOR_DOMAIN] =
  g_param_spec_object("list-stor-domain",
                      "list-stor-domain",
                      "gtkliststor proprety for domains",
                      GTK_TYPE_LIST_STORE,
                      G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (object_class, N_PROPS, properties);

}

static void
posman_panel_list_init (PosmanPanelList *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

posmanpanellistview
posman_panel_list_get_view(PosmanPanelList *self)
{
  return self->view;
}

void
posman_panel_list_set_view(PosmanPanelList *self,
                            posmanpanellistview view)
{
  g_return_if_fail (POSMAN_IS_PANEL_LIST (self));

  if(self->view == view)
    return;

  gboolean crossfade = view == posman_panel_list_add_cust || self->view == posman_panel_list_add_cust;

  self->view = view;

  gtk_stack_set_transition_type(GTK_STACK (self),
                                crossfade ? GTK_STACK_TRANSITION_TYPE_CROSSFADE : GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);

  gtk_stack_set_visible_child (GTK_STACK (self),
                               get_listbox_from_view (self, view));

  g_object_notify_by_pspec (G_OBJECT (self),
                            properties[PROP_VIEW]);
}

void
posman_panel_list_remove_row_cust(PosmanPanelList     *self,
                                  PosmanPanelRowCust  *row)
{
  gtk_container_remove(GTK_CONTAINER (self->cust_listbox),
                       GTK_WIDGET (row));
}

void
posman_panel_list_set_model_domain(PosmanPanelList  *self,
                                   GtkListStore     *list_stor)
{
  g_return_if_fail (POSMAN_IS_PANEL_LIST (self));
  g_return_if_fail (list_stor == NULL || GTK_IS_LIST_STORE (list_stor));

  if (self->list_stor_domain)
    g_object_unref(self->list_stor_domain);

  self->list_stor_domain = (GtkWidget *) g_object_ref (list_stor);
  if(self->list_stor_domain)
    gtk_combo_box_set_model(GTK_COMBO_BOX (self->domain_combobox),
                            GTK_TREE_MODEL (list_stor));

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_STOR_DOMAIN]);
}

static GtkWidget *
create_widget_func(gpointer item,
                   gpointer user_data)
{
  GtkWidget  *row = (GtkWidget*)item;

  return row;
}

void
posman_panel_list_set_list_stor_cust(PosmanPanelList *self,
                                     GObject         *list_stor)
{
  g_return_if_fail (POSMAN_IS_PANEL_LIST (self));
  g_return_if_fail (list_stor == NULL || G_IS_LIST_STORE (list_stor));

  if (self->list_stor_cust)
    g_object_unref(self->list_stor_cust);

  self->list_stor_cust = g_object_ref (list_stor);

  if(self->list_stor_cust)
    gtk_list_box_bind_model (GTK_LIST_BOX (self->cust_listbox),
                             G_LIST_MODEL (list_stor),
                             create_widget_func,
                             NULL,NULL);

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_STOR_CUST]);
}

gboolean
posman_panel_list_is_add_cust_in_prog(PosmanPanelList *self)
{
  gboolean        in_prog;
  GtkEntryBuffer   *buffer;
  buffer  = gtk_entry_get_buffer (GTK_ENTRY (self->name_entry));
  in_prog = gtk_entry_buffer_get_length(buffer) > 0;
  return in_prog;
}

void
posman_panel_list_clear_add_cust(PosmanPanelList *self)
{
  gtk_entry_set_text(GTK_ENTRY(self->name_entry),"");
  gtk_entry_set_text(GTK_ENTRY(self->adress_entry),"");
  gtk_entry_set_text(GTK_ENTRY(self->phone_entry),"");
  gtk_combo_box_set_active(GTK_COMBO_BOX (self->domain_combobox),-1);
  gtk_text_buffer_set_text(gtk_text_view_get_buffer (GTK_TEXT_VIEW (self->description_textview)),
                           "",
                           -1);
}
