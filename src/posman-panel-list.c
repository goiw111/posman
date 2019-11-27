#include "posman-panel-list.h"
#include "posman-list-box-data.h"
#include <sqlite3.h>

struct _PosmanPanelList
{
  GtkStack                  parent_instance;

  GtkWidget                 *cust_listbox;
  GtkWidget                 *cmnd_listbox;

  posmanpanellistview       view;

  GObject                   *list_stor_cust;
  GObject                   *list_stor_cmnd;
};

G_DEFINE_TYPE (PosmanPanelList, posman_panel_list, GTK_TYPE_STACK)

enum {
  PROP_0,
  PROP_VIEW,
  PROP_STOR_CUST,
  PROP_STOR_CMND,
  N_PROPS
};

enum {
  ROW_ACTIVATED,
  LAST_SIGNAL
};

static GParamSpec *properties [N_PROPS] = { NULL, };
static guint signals[LAST_SIGNAL] = { 0 };

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

    default:
      return NULL;
    }
}




/*callback*/

static void
cmnd_row_activated_cb(GtkListBox      *box,
                      GtkListBoxRow   *row,
                      PosmanPanelList *self)
{

}

static void
cust_row_activated_cb(GtkListBox      *box,
                      GtkListBoxRow   *row,
                      PosmanPanelList *self)
{
  g_signal_emit (self, signals[ROW_ACTIVATED], 0, box, row);
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

  if (self->list_stor_cust)
    g_object_unref(self->list_stor_cust);
  if (self->list_stor_cmnd)
    g_object_unref(self->list_stor_cmnd);

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

      case PROP_STOR_CUST:
        posman_panel_list_set_model_cust(self, g_value_get_object(value));
        break;

      case PROP_STOR_CMND:
        posman_panel_list_set_model_cmnd(self, g_value_get_object(value));
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
                                        cust_listbox);
  gtk_widget_class_bind_template_child (widget_class,
                                        PosmanPanelList,
                                        cmnd_listbox);

  gtk_widget_class_bind_template_callback (widget_class,
                                           cmnd_row_activated_cb);
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

  g_object_class_install_properties (object_class, N_PROPS, properties);

  signals[ROW_ACTIVATED] =
    g_signal_new ("row-activated",
                  G_TYPE_FROM_CLASS (object_class),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL, NULL,
                  NULL,
                  G_TYPE_NONE, 2,
                  GTK_TYPE_LIST_BOX,
                  GTK_TYPE_LIST_BOX_ROW);

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
posman_panel_list_set_model_cust(PosmanPanelList *self,
                                 GObject         *list_stor)
{
  g_return_if_fail (POSMAN_IS_PANEL_LIST (self));
  g_return_if_fail (list_stor == NULL || G_IS_OBJECT (list_stor));

  if (self->list_stor_cust)
    g_object_unref(self->list_stor_cust);


  self->list_stor_cust = g_object_ref (list_stor);

  if (self->list_stor_cust)
      gtk_list_box_bind_model(GTK_LIST_BOX (self->cust_listbox),
                              G_LIST_MODEL (self->list_stor_cust),
                              row_cust_data_create,
                              NULL,NULL);

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_STOR_CUST]);
}

void
posman_panel_list_set_model_cmnd(PosmanPanelList *self,
                                 GObject         *list_stor)
{
  g_return_if_fail (POSMAN_IS_PANEL_LIST (self));
  g_return_if_fail (list_stor == NULL || G_IS_OBJECT (list_stor));

  if (self->list_stor_cmnd)
    g_object_unref(self->list_stor_cmnd);

  self->list_stor_cmnd = g_object_ref (list_stor);

  if (self->list_stor_cmnd)
      gtk_list_box_bind_model(GTK_LIST_BOX (self->cmnd_listbox),
                              G_LIST_MODEL (self->list_stor_cmnd),
                              row_cmnd_data_create,
                              NULL,NULL);


  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_STOR_CMND]);
}
