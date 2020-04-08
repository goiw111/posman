#include "posman-item-list-row.h"

struct _PosmanItemListRow
{
  GtkListBoxRow parent_instance;

  gint          item_id;
  gint          in_order_id;

  GtkWidget    *item_price;
  GtkWidget    *item_qt;

  GtkWidget    *price_entry;
  GtkWidget    *qt_entry;
  GtkWidget    *qt_pack_entry;

  GtkWidget    *price_label;
  GtkWidget    *item_label;

  gint          qt_pack_unit;
};

G_DEFINE_TYPE (PosmanItemListRow, posman_item_list_row, GTK_TYPE_LIST_BOX_ROW)

enum {
  PROP_0,
  PROP_ID,
  PROP_PRICE,
  PROP_QT,
  PROP_QT_PACK,
  PROP_IN_ORDER_ID,
  N_PROPS
};

enum {
  VALUE,

  LAST_SIGNAL
};

static GParamSpec *properties [N_PROPS];
static guint       signals[LAST_SIGNAL];

static void
destroy_button_clicked_cb(GtkButton *button,
                          gpointer   user_data)
{
  PosmanItemListRow   *self = (PosmanItemListRow *)user_data;

  gtk_widget_destroy (GTK_WIDGET(self));
}

static void
value_cb(GtkSpinButton *spin_button,
         gpointer       user_data)
{
  PosmanItemListRow   *self = (PosmanItemListRow *)user_data;

  g_signal_emit (self, signals[VALUE],0);
}

static gboolean
price_value_to_label(GBinding     *binding,
                     const GValue *from_value,
                     GValue       *to_value,
                     gpointer      user_data)
{
  PosmanItemListRow   *self = (PosmanItemListRow *)user_data;
  g_autofree gchar   *buffer = NULL;

  buffer = g_strdup_printf ("%.2f",
                              gtk_spin_button_get_value(GTK_SPIN_BUTTON (self->price_entry))
                            * gtk_spin_button_get_value(GTK_SPIN_BUTTON (self->qt_entry)));

  g_value_set_string (to_value,buffer);

  return TRUE;
}

PosmanItemListRow *
posman_item_list_row_new (gint    item_id,
                          GtkAdjustment  *adjustment_price,
                          GtkAdjustment  *adjustment_qt,
                          gint            qt_pack_unit,
                          gchar          *name,
                          gint            id)
{
  PosmanItemListRow   *self;

  self = g_object_new (POSMAN_TYPE_ITEM_LIST_ROW,
                       "item-id",item_id,
                       "id",id,
                       "item-price",adjustment_price,
                       "item-qt",adjustment_qt,
                       "item-qt-pack",qt_pack_unit,
                       NULL);

  GtkAdjustment     *adj;

  adj = gtk_adjustment_new (gtk_adjustment_get_value (gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON (self->qt_entry))),
                            gtk_adjustment_get_lower (gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON (self->qt_entry))),
                            gtk_adjustment_get_upper (gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON (self->qt_entry))),
                            (gint)qt_pack_unit,
                            1, 0);
  gtk_spin_button_set_adjustment (GTK_SPIN_BUTTON (self->qt_pack_entry), adj);

  gtk_label_set_text (GTK_LABEL (self->item_label), name);

  return self;
}

static void
posman_item_list_row_finalize (GObject *object)
{
  PosmanItemListRow *self = (PosmanItemListRow *)object;

  G_OBJECT_CLASS (posman_item_list_row_parent_class)->finalize (object);
}

static void
posman_item_list_row_dispose(GObject *object)
{
  PosmanItemListRow *self = (PosmanItemListRow *)object;

  G_OBJECT_CLASS (posman_item_list_row_parent_class)->dispose (object);
}

static void
posman_item_list_row_constructed(GObject *object)
{
  PosmanItemListRow *self = (PosmanItemListRow *)object;

  G_OBJECT_CLASS (posman_item_list_row_parent_class)->constructed (object);

  g_object_bind_property_full(self->price_entry,"value",
                              self->price_label,"label",
                              G_BINDING_DEFAULT | G_BINDING_SYNC_CREATE,
                              price_value_to_label,
                              NULL,
                              self,NULL);
  g_object_bind_property_full(self->qt_entry,"value",
                              self->price_label,"label",
                              G_BINDING_DEFAULT | G_BINDING_SYNC_CREATE,
                              price_value_to_label,
                              NULL,
                              self,NULL);

  g_signal_connect (self->price_entry, "value-changed", G_CALLBACK (value_cb), self);
  g_signal_connect (self->qt_entry,    "value-changed", G_CALLBACK (value_cb), self);
}

static void
posman_item_list_row_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  PosmanItemListRow *self = POSMAN_ITEM_LIST_ROW (object);

  switch (prop_id)
    {
      case PROP_ID:
        g_value_set_int (value,posman_item_list_get_id(self));
        break;
      case PROP_PRICE:
        g_value_set_object (value,posman_item_list_get_price (self));
        break;
      case PROP_QT:
        g_value_set_object (value,posman_item_list_get_qt (self));
        break;
      case PROP_QT_PACK:
        g_value_set_int (value,posman_item_list_get_qt_pack (self));
        break;
      case PROP_IN_ORDER_ID:
        g_value_set_int (value,posman_item_list_get_in_order_id(self));
        break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
posman_item_list_row_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  PosmanItemListRow *self = POSMAN_ITEM_LIST_ROW (object);

  switch (prop_id)
    {
      case PROP_ID:
        posman_item_list_set_id(self,g_value_get_int(value));
        break;
      case PROP_PRICE:
        posman_item_list_set_price (self, g_value_get_object(value));
        break;
      case PROP_QT:
        posman_item_list_set_qt (self, g_value_get_object(value));
        break;
      case PROP_QT_PACK:
        posman_item_list_set_qt_pack (self, g_value_get_int(value));
        break;
      case PROP_IN_ORDER_ID:
        posman_item_list_set_in_order_id (self, g_value_get_int(value));
        break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
posman_item_list_row_class_init (PosmanItemListRowClass *klass)
{
  GObjectClass    *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = posman_item_list_row_finalize;
  object_class->dispose = posman_item_list_row_dispose;
  object_class->constructed = posman_item_list_row_constructed;
  object_class->get_property = posman_item_list_row_get_property;
  object_class->set_property = posman_item_list_row_set_property;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/pos/manager/posman-item-list-row.ui");

  gtk_widget_class_bind_template_child (widget_class,PosmanItemListRow,price_entry);
  gtk_widget_class_bind_template_child (widget_class,PosmanItemListRow,qt_entry);
  gtk_widget_class_bind_template_child (widget_class,PosmanItemListRow,qt_pack_entry);
  gtk_widget_class_bind_template_child (widget_class,PosmanItemListRow,price_label);
  gtk_widget_class_bind_template_child (widget_class,PosmanItemListRow,item_label);

  gtk_widget_class_bind_template_callback (widget_class, destroy_button_clicked_cb);

  properties[PROP_ID] = g_param_spec_int ("item-id",
                                          "item-id",
                                          "the item id",
                                          -1,G_MAXINT,-1,
                                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_IN_ORDER_ID] = g_param_spec_int ("id",
                                                   "id",
                                                   "the item id in order",
                                                   -1,G_MAXINT,-1,
                                                   G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_PRICE] = g_param_spec_object("item-price",
                                               "item-price",
                                               "the item price",
                                               GTK_TYPE_ADJUSTMENT,
                                               G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_QT] = g_param_spec_object("item-qt",
                                            "item-qt",
                                            "the item qt",
                                            GTK_TYPE_ADJUSTMENT,
                                            G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_QT_PACK] = g_param_spec_int("item-qt-pack",
                                              "item-qt-pack",
                                              "how muche pack item",
                                              -1,G_MAXINT,-1,
                                              G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (object_class, N_PROPS, properties);

  signals[VALUE] = g_signal_new("value",
                                G_OBJECT_CLASS_TYPE (object_class),
                                G_SIGNAL_RUN_FIRST,
                                0,NULL,NULL,NULL,
                                G_TYPE_NONE,0);
}

static void
posman_item_list_row_init (PosmanItemListRow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

gint
posman_item_list_get_id(PosmanItemListRow *self)
{
  return self->item_id;
}

void
posman_item_list_set_id(PosmanItemListRow    *self,gint  item_id)
{
  self->item_id = item_id;

  g_object_notify_by_pspec (G_OBJECT (self),properties[PROP_ID]);
}

gint
posman_item_list_get_in_order_id(PosmanItemListRow *self)
{
  return self->in_order_id;
}

void
posman_item_list_set_in_order_id(PosmanItemListRow    *self,gint  in_order_id)
{
  self->in_order_id = in_order_id;

  g_object_notify_by_pspec (G_OBJECT (self),properties[PROP_IN_ORDER_ID]);
}

void
posman_item_list_set_price(PosmanItemListRow    *self,GtkAdjustment   *adjustment)
{
  self->item_price = g_object_ref(G_OBJECT(adjustment));

  gtk_spin_button_set_adjustment (GTK_SPIN_BUTTON (self->price_entry), adjustment);

  g_object_notify_by_pspec (G_OBJECT (self),properties[PROP_PRICE]);
}

GtkWidget *
posman_item_list_get_price(PosmanItemListRow    *self)
{
  return self->item_price;
}

void
posman_item_list_set_qt(PosmanItemListRow    *self,GtkAdjustment   *adjustment)
{
  self->item_qt = g_object_ref(G_OBJECT(adjustment));

  gtk_spin_button_set_adjustment (GTK_SPIN_BUTTON (self->qt_entry), adjustment);

  g_object_notify_by_pspec (G_OBJECT (self),properties[PROP_QT]);
}

GtkWidget *
posman_item_list_get_qt(PosmanItemListRow    *self)
{
  return self->item_qt;
}

void
posman_item_list_set_qt_pack(PosmanItemListRow    *self,gint   qt_pack_unit)
{
  self->qt_pack_unit = qt_pack_unit;

  g_object_notify_by_pspec (G_OBJECT (self),properties[PROP_QT]);
}

gint
posman_item_list_get_qt_pack(PosmanItemListRow    *self)
{
  return self->qt_pack_unit;
}
