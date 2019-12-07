#include "posman-action-menu-cmnd.h"

struct _PosmanActionMenuCmnd
{
  GtkMenuButton parent_instance;
};

G_DEFINE_TYPE (PosmanActionMenuCmnd, posman_action_menu_cmnd, GTK_TYPE_MENU_BUTTON)

enum {
  PROP_0,
  N_PROPS
};

enum {
  ADD_PRESSED,
  REMOVE_PRESSED,

  LAST_SIGNAL
};

static GParamSpec *properties [N_PROPS];
static guint      menu_signals[LAST_SIGNAL];

PosmanActionMenuCmnd *
posman_action_menu_cmnd_new (void)
{
  return g_object_new (POSMAN_TYPE_ACTION_MENU_CMND, NULL);
}

static void
posman_action_menu_cmnd_finalize (GObject *object)
{
  PosmanActionMenuCmnd *self = (PosmanActionMenuCmnd *)object;

  G_OBJECT_CLASS (posman_action_menu_cmnd_parent_class)->finalize (object);
}

static void
posman_action_menu_cmnd_get_property (GObject    *object,
                                      guint       prop_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
  PosmanActionMenuCmnd *self = POSMAN_ACTION_MENU_CMND (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
posman_action_menu_cmnd_set_property (GObject      *object,
                                      guint         prop_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
  PosmanActionMenuCmnd *self = POSMAN_ACTION_MENU_CMND (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
posman_action_menu_cmnd_class_init (PosmanActionMenuCmndClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);


  object_class->finalize = posman_action_menu_cmnd_finalize;
  object_class->get_property = posman_action_menu_cmnd_get_property;
  object_class->set_property = posman_action_menu_cmnd_set_property;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/pos/manager/posman-action-menu-cmnd.ui");

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
posman_action_menu_cmnd_add_activate(GSimpleAction     *simple,
                                GVariant          *parameter,
                                gpointer          user_data)
{
  PosmanActionMenuCmnd *menu = POSMAN_ACTION_MENU_CMND (user_data);

  g_signal_emit (menu, menu_signals[ADD_PRESSED],0);
}

static void
posman_action_menu_cmnd_remove_activate(GSimpleAction    *simple,
                                   GVariant         *parameter,
                                   gpointer         user_data)
{
  PosmanActionMenuCmnd *menu = POSMAN_ACTION_MENU_CMND (user_data);

  g_signal_emit (menu, menu_signals[REMOVE_PRESSED],0);
}

static void
posman_action_menu_cmnd_init (PosmanActionMenuCmnd *self)
{
  const GActionEntry entries[] = {
    { "add",    posman_action_menu_cmnd_add_activate   },
    { "remove", posman_action_menu_cmnd_remove_activate}
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
