#include "posman-panel-list.h"
#include "posman-panel-model.h"
#include "posman-panel-loader.h"
#include <sqlite3.h>

struct _PosmanPanelList
{
  GtkStack                  parent_instance;

  GtkWidget                 *main_listbox;

  GtkWidget                 *cust_listbox;

  posmanpanellistview       view;

  GObject                   *list_stor;


};

G_DEFINE_TYPE (PosmanPanelList, posman_panel_list, GTK_TYPE_STACK)

enum {
  PROP_0,
  PROP_VIEW,
  PROP_STOR,
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
      return self->main_listbox;

    case posman_panel_list_cust:
      return self->cust_listbox;

    default:
      return NULL;
    }
}




/*callback*/

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

  if (self->list_stor)
    g_object_unref(self->list_stor);

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

      case PROP_STOR:
        posman_panel_list_set_model(self, g_value_get_object(value));
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
                    G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_STOR] =
  g_param_spec_object("list-stor",
                      "list-stor",
                      "GListStor proprety",
                      G_TYPE_LIST_STORE,
                      G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (object_class, N_PROPS, properties);

}

static void
posman_panel_list_init (PosmanPanelList *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
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

posmanpanellistview
posman_panel_list_get_view(PosmanPanelList *self)
{
  g_return_val_if_fail (POSMAN_IS_PANEL_LIST (self), -1);
  return self->view;
}

void
posman_panel_list_set_model(PosmanPanelList *self,
                            GObject         *list_stor)
{
  g_return_if_fail (POSMAN_IS_PANEL_LIST (self));
  g_return_if_fail (list_stor == NULL || G_IS_OBJECT (list_stor));

  if (self->list_stor)
    g_object_unref(self->list_stor);

  self->list_stor = list_stor;

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_STOR]);
}
