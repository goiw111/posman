#include "posman-item-chooser.h"
#include "posman-item.h"

struct _PosmanItemChooser
{
  GtkOverlay  parent_instance;

  GtkWidget   *box_cat_item;
  GtkWidget   *box_stock;
  GtkWidget   *revealer;
};

G_DEFINE_TYPE (PosmanItemChooser, posman_item_chooser, GTK_TYPE_OVERLAY)

enum {
  PROP_0,
  PROP_STOCK,
  PROP_ITEMS,
  N_PROPS
};

enum {
  ITEM_ACTIVATED,
  STOCK_ACTIVATED,
  GO_HOME,

  LAST_SIGNAL
};

static GParamSpec *properties [N_PROPS];
static guint       signals[LAST_SIGNAL];

/*callback*/

void
box_cat_item_child_activated_cb(GtkFlowBox      *box,
                                GtkFlowBoxChild *child,
                                gpointer         user_data)
{
  PosmanItemChooser *self = (PosmanItemChooser *)user_data;
  g_signal_emit(self,signals[ITEM_ACTIVATED],0,box,child);
}

static void
box_stock_child_activated_cb(GtkFlowBox      *box,
                             GtkFlowBoxChild *child,
                             gpointer         user_data)
{
  PosmanItemChooser *self = (PosmanItemChooser *)user_data;
  g_signal_emit(self,signals[STOCK_ACTIVATED],0,box,child);
}

void
go_home_clicked_cb(GtkButton    *button,
                   gpointer      user_data)
{
  PosmanItemChooser *self = (PosmanItemChooser *)user_data;
  g_signal_emit(self,signals[GO_HOME],0);
}

PosmanItemChooser *
posman_item_chooser_new (void)
{
  return g_object_new (POSMAN_TYPE_ITEM_CHOOSER, NULL);
}

static void
posman_item_chooser_finalize (GObject *object)
{
  PosmanItemChooser *self = (PosmanItemChooser *)object;

  G_OBJECT_CLASS (posman_item_chooser_parent_class)->finalize (object);
}

static void
posman_item_chooser_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  PosmanItemChooser *self = POSMAN_ITEM_CHOOSER (object);

  switch (prop_id)
    {
      case PROP_ITEMS:
        posman_item_chooser_get_items_flow(self);
        break;
      case PROP_STOCK:
        posman_item_chooser_get_stock_flow(self);
        break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
posman_item_chooser_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  PosmanItemChooser *self = POSMAN_ITEM_CHOOSER (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
posman_item_chooser_class_init (PosmanItemChooserClass *klass)
{
  GObjectClass    *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = posman_item_chooser_finalize;
  object_class->get_property = posman_item_chooser_get_property;
  object_class->set_property = posman_item_chooser_set_property;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/pos/manager/posman-item-chooser.ui");
  gtk_widget_class_bind_template_child (widget_class,  PosmanItemChooser, box_cat_item);
  gtk_widget_class_bind_template_child (widget_class,  PosmanItemChooser, box_stock);
   gtk_widget_class_bind_template_child (widget_class, PosmanItemChooser, revealer);

  gtk_widget_class_bind_template_callback (widget_class, box_cat_item_child_activated_cb);
  gtk_widget_class_bind_template_callback (widget_class, box_stock_child_activated_cb);
  gtk_widget_class_bind_template_callback (widget_class, go_home_clicked_cb);

  properties[PROP_ITEMS] = g_param_spec_object ("items",
                                                "items",
                                                "a gtkflowbox contain the items",
                                                GTK_TYPE_FLOW_BOX,
                                                G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_STOCK] = g_param_spec_object ("stock",
                                                "stock",
                                                "a gtkflowbox contain the stock items",
                                                GTK_TYPE_FLOW_BOX,
                                                G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (object_class, N_PROPS, properties);

  signals[ITEM_ACTIVATED] = g_signal_new("item-activated",
                                         G_OBJECT_CLASS_TYPE (object_class),
                                         G_SIGNAL_RUN_FIRST,0,NULL,NULL,NULL,
                                         G_TYPE_NONE,2,
                                         GTK_TYPE_FLOW_BOX,
                                         GTK_TYPE_FLOW_BOX_CHILD);

  signals[STOCK_ACTIVATED] = g_signal_new("stock-activated",
                                         G_OBJECT_CLASS_TYPE (object_class),
                                         G_SIGNAL_RUN_FIRST,0,NULL,NULL,NULL,
                                         G_TYPE_NONE,2,
                                         GTK_TYPE_FLOW_BOX,
                                         GTK_TYPE_FLOW_BOX_CHILD);

  signals[GO_HOME] = g_signal_new("go-home",
                                  G_OBJECT_CLASS_TYPE (object_class),
                                  G_SIGNAL_RUN_FIRST,0,NULL,NULL,NULL,
                                  G_TYPE_NONE,0);

  g_type_ensure (POSMAN_TYPE_ITEM);
}

static void
posman_item_chooser_init (PosmanItemChooser *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

GtkFlowBox *
posman_item_chooser_get_items_flow(PosmanItemChooser   *self)
{
  return  GTK_FLOW_BOX (self->box_cat_item);
}

GtkFlowBox *
posman_item_chooser_get_stock_flow(PosmanItemChooser   *self)
{
  return  GTK_FLOW_BOX (self->box_stock);
}

void
posman_item_chooser_set_stock_visible(PosmanItemChooser   *self,gboolean  visible)
{
  gtk_revealer_set_reveal_child(GTK_REVEALER (self->revealer),visible);
}
