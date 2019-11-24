#include "posman-action-menu.h"

struct _PosmanActionMenu
{
  GtkMenuButton parent_instance;
};

G_DEFINE_TYPE (PosmanActionMenu, posman_action_menu, GTK_TYPE_MENU_BUTTON)

enum {
  PROP_0,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

PosmanActionMenu *
posman_action_menu_new (void)
{
  return g_object_new (POSMAN_TYPE_ACTION_MENU, NULL);
}

static void
posman_action_menu_finalize (GObject *object)
{
  PosmanActionMenu *self = (PosmanActionMenu *)object;

  G_OBJECT_CLASS (posman_action_menu_parent_class)->finalize (object);
}

static void
posman_action_menu_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  PosmanActionMenu *self = POSMAN_ACTION_MENU (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
posman_action_menu_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  PosmanActionMenu *self = POSMAN_ACTION_MENU (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
posman_action_menu_class_init (PosmanActionMenuClass *klass)
{
  GObjectClass    *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = posman_action_menu_finalize;
  object_class->get_property = posman_action_menu_get_property;
  object_class->set_property = posman_action_menu_set_property;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/pos/manager/posman-action-menu.ui");
}

/* entries activate */

static void
posman_action_menu_add_activate(GSimpleAction *simple,
                                GVariant      *parameter,
                                gpointer       user_data)
{
  g_print("add\n");
}

static void
posman_action_menu_remove_activate(GSimpleAction *simple,
                                GVariant      *parameter,
                                gpointer       user_data)
{
  g_print("remove\n");
}


static void
posman_action_menu_init (PosmanActionMenu *self)
{
  GActionGroup *actions;
  static const GActionEntry
  entries[] = {
    { "add", posman_action_menu_add_activate, NULL, NULL, NULL },
    { "remove", posman_action_menu_remove_activate, NULL, NULL, NULL }
  };

  gtk_widget_init_template (GTK_WIDGET (self));
  actions = (GActionGroup*)g_simple_action_group_new ();
  g_action_map_add_action_entries (G_ACTION_MAP (actions),
                                       entries, 2,
                                       self);
  gtk_widget_insert_action_group (GTK_WIDGET (self), "menu", actions);
}