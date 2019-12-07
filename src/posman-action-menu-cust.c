#include "posman-action-menu-cust.h"

struct _PosmanActionMenuCust
{
  GtkMenuButton parent_instance;

  GtkWidget     *remove_button;
  GtkWidget     *remove_symbol;
};

G_DEFINE_TYPE (PosmanActionMenuCust, posman_action_menu_cust, GTK_TYPE_MENU_BUTTON)

enum {
  PROP_0,
  PROP_REMOVE_BUTTON,
  N_PROPS
};

enum {
  ADD_PRESSED,
  REMOVE_PRESSED,

  LAST_SIGNAL
};

static GParamSpec *properties [N_PROPS];
static guint      menu_signals[LAST_SIGNAL];

PosmanActionMenuCust *
posman_action_menu_cust_new (void)
{
  return g_object_new (POSMAN_TYPE_ACTION_MENU_CUST, NULL);
}

static void
posman_action_menu_cust_finalize (GObject *object)
{
  PosmanActionMenuCust *self = (PosmanActionMenuCust *)object;

  G_OBJECT_CLASS (posman_action_menu_cust_parent_class)->finalize (object);
}

static void
posman_action_menu_cust_get_property (GObject    *object,
                                      guint       prop_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
  PosmanActionMenuCust *self = POSMAN_ACTION_MENU_CUST (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
posman_action_menu_cust_set_property (GObject      *object,
                                      guint         prop_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
  PosmanActionMenuCust *self = POSMAN_ACTION_MENU_CUST (object);

  switch (prop_id)
    {
      case PROP_REMOVE_BUTTON:
        posman_action_menu_cust_set_remove_button(self,
                                                  g_value_get_object (value));
        break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
posman_action_menu_cust_class_init (PosmanActionMenuCustClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = posman_action_menu_cust_finalize;
  object_class->get_property = posman_action_menu_cust_get_property;
  object_class->set_property = posman_action_menu_cust_set_property;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/pos/manager/posman-action-menu-cust.ui");
  gtk_widget_class_bind_template_child (widget_class, PosmanActionMenuCust, remove_symbol);

  properties[PROP_REMOVE_BUTTON] =
  g_param_spec_object("remove-button",
                      "remove-button",
                      "remove button for binding",
                      GTK_TYPE_BUTTON,
                      G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (object_class, N_PROPS, properties);

  menu_signals[ADD_PRESSED] = g_signal_new("add_pressed",
                              G_OBJECT_CLASS_TYPE (object_class),
                              G_SIGNAL_RUN_FIRST,
                              0,
                              NULL, NULL,
                              NULL,
                              G_TYPE_NONE, 0);

  menu_signals[REMOVE_PRESSED] = g_signal_new("remove_pressed",
                              G_OBJECT_CLASS_TYPE (object_class),
                              G_SIGNAL_RUN_FIRST,
                              0,
                              NULL, NULL,
                              NULL,
                              G_TYPE_NONE, 0);
}

static void
posman_action_menu_cust_add_activate(GSimpleAction     *simple,
                                GVariant          *parameter,
                                gpointer          user_data)
{
  PosmanActionMenuCust *menu = POSMAN_ACTION_MENU_CUST (user_data);

  g_signal_emit (menu, menu_signals[ADD_PRESSED],0);
}

static void
posman_action_menu_cust_remove_activate(GSimpleAction    *simple,
                                   GVariant         *parameter,
                                   gpointer         user_data)
{
  PosmanActionMenuCust *menu = POSMAN_ACTION_MENU_CUST (user_data);

  g_signal_emit (menu, menu_signals[REMOVE_PRESSED],0);
}


static void
posman_action_menu_cust_init (PosmanActionMenuCust *self)
{
  const GActionEntry entries[] = {
    { "add",    posman_action_menu_cust_add_activate   },
    { "remove", posman_action_menu_cust_remove_activate}
  };
  GSimpleActionGroup *group;

  gtk_widget_init_template (GTK_WIDGET (self));
  group = g_simple_action_group_new();
  g_action_map_add_action_entries (G_ACTION_MAP (group),
                                   entries,
                                   G_N_ELEMENTS (entries),
                                   self);

  gtk_widget_insert_action_group(GTK_WIDGET (self),
                                 "menu",
                                 G_ACTION_GROUP (group));
}

void
posman_action_menu_cust_set_remove_button(PosmanActionMenuCust *self,
                                          GtkButton            *button)
{
  g_return_if_fail (POSMAN_IS_ACTION_MENU_CUST (self));
  g_return_if_fail (button == NULL || GTK_IS_BUTTON (button));

  if(self->remove_button)
    g_object_unref(self->remove_button);

  self->remove_button = (GtkWidget *) g_object_ref(button);
  if(self->remove_button)
    g_object_bind_property (button,
                            "visible",
                            self->remove_symbol,
                            "sensitive",
                            G_BINDING_BIDIRECTIONAL | G_BINDING_SYNC_CREATE | G_BINDING_INVERT_BOOLEAN);

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_REMOVE_BUTTON]);
}
