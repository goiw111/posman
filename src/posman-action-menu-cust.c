#include "posman-action-menu-cust.h"

struct _PosmanActionMenuCust
{
  GtkMenuButton parent_instance;

};

G_DEFINE_TYPE (PosmanActionMenuCust, posman_action_menu_cust, GTK_TYPE_MENU_BUTTON)

enum {
  PROP_0,
  N_PROPS
};

enum {
  LAST_SIGNAL
};

static GParamSpec *properties [N_PROPS];

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

}

static void
posman_action_menu_cust_init (PosmanActionMenuCust *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}
