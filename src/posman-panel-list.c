#include "posman-panel-list.h"

struct _PosmanPanelList
{
  GtkStack    parent_instance;

  GtkWidget                 *main_listbox;
  GtkWidget                 *cust_listbox;

  posmanpanellistview       view;
  gint                      cust_id;
};
struct {
  GtkWidget                 *row;

  gint                      id;
  gchar                     *name;
  gint                      *total;

}RowMainData;

struct {
  GtkWidget                 *row;

  gint                      commond_id;
}RowCustData;

G_DEFINE_TYPE (PosmanPanelList, posman_panel_list, GTK_TYPE_STACK)

enum {
  PROP_0,
  PROP_VIEW,
  N_PROPS
};

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

/*
 * RowData functions
 */

static
row_data_new()
{

}

static GParamSpec *properties [N_PROPS] = { NULL, };

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

static void
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

  properties[PROP_VIEW] = g_param_spec_int ("view",
                                            "view",
                                            "the current view of the sidelist",
                                            posman_panel_list_main ,
                                            posman_panel_list_cust ,
                                            posman_panel_list_main ,
                                            G_PARAM_READWRITE );
  g_object_class_install_properties (object_class, N_PROPS, properties);

}

static void
posman_panel_list_init (PosmanPanelList *self)
{
}
