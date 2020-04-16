#include "posman-add-payment.h"

struct _PosmanAddPayment
{
  GtkBox       parent_instance;

  GtkWidget   *liststore1;

  gint         cust_id;
};

G_DEFINE_TYPE (PosmanAddPayment, posman_add_payment, GTK_TYPE_BOX)

enum {
  PROP_0,
  PROP_MODEL,
  PROP_CUST_ID,
  N_PROPS
};

enum {
  LOAD_PAYMENTS,

  LAST_SIGNAL
};

static GParamSpec *properties [N_PROPS];
static guint       signals[LAST_SIGNAL];

PosmanAddPayment *
posman_add_payment_new (void)
{
  return g_object_new (POSMAN_TYPE_ADD_PAYMENT, NULL);
}

static void
posman_add_payment_finalize (GObject *object)
{
  PosmanAddPayment *self = (PosmanAddPayment *)object;

  G_OBJECT_CLASS (posman_add_payment_parent_class)->finalize (object);
}

static void
posman_add_payment_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  PosmanAddPayment *self = POSMAN_ADD_PAYMENT (object);

  switch (prop_id)
    {
      case PROP_CUST_ID:
        g_value_set_int (value, posman_add_payment_get_cust_id(self));
        break;
      case PROP_MODEL:
        g_value_set_object (value, posman_add_payment_get_model (self));
        break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
posman_add_payment_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  PosmanAddPayment *self = POSMAN_ADD_PAYMENT (object);

  switch (prop_id)
    {
      case PROP_CUST_ID:
        posman_add_payment_set_cust_id (self,g_value_get_int (value));
        break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
posman_add_payment_class_init (PosmanAddPaymentClass *klass)
{
  GObjectClass    *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = posman_add_payment_finalize;
  object_class->get_property = posman_add_payment_get_property;
  object_class->set_property = posman_add_payment_set_property;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/pos/manager/posman-add-payment.ui");

  gtk_widget_class_bind_template_child (widget_class,PosmanAddPayment,liststore1);

  properties[PROP_MODEL] =
  g_param_spec_object("pay-model",
                      "payments model",
                      "the model that contain the payment",
                      GTK_TYPE_TREE_MODEL,
                      G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY );

  properties[PROP_CUST_ID] =
  g_param_spec_object("cust-id",
                      "cust-id",
                      "the customer id",
                      GTK_TYPE_TREE_MODEL,
                      G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY );

   g_object_class_install_properties (object_class, N_PROPS, properties);

  signals[LOAD_PAYMENTS] = g_signal_new("value",
                                        G_OBJECT_CLASS_TYPE (object_class),
                                        G_SIGNAL_RUN_FIRST,
                                        0,NULL,NULL,NULL,
                                        G_TYPE_NONE,1,
                                        GTK_TYPE_LIST_STORE);
}

static void
posman_add_payment_init (PosmanAddPayment *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

  gtk_list_store_insert_with_values(self->liststore1,
                                    NULL,-1,
                                    0,"ggg",
                                    1,"ggg",
                                    2,"ggg",-1);
}

void
posman_add_payment_load_payments(PosmanAddPayment *self)
{
  g_return_if_fail (POSMAN_IS_ADD_PAYMENT (self));

  g_signal_emit (self, signals[LOAD_PAYMENTS],0,
                 self->liststore1);
}

void
posman_add_payment_set_cust_id(PosmanAddPayment   *self,gint  cust_id)
{
  g_return_if_fail (POSMAN_IS_ADD_PAYMENT (self));

  if(cust_id >= -1)
    self->cust_id = cust_id;
}

gint
posman_add_payment_get_cust_id (PosmanAddPayment   *self)
{
  g_return_val_if_fail (POSMAN_IS_ADD_PAYMENT (self),-1);

  return self->cust_id;
}

GtkWidget *
posman_add_payment_get_model (PosmanAddPayment   *self)
{
  g_return_val_if_fail (POSMAN_IS_ADD_PAYMENT (self),NULL);

  return self->liststore1;
}
