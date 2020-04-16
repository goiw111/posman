#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define POSMAN_TYPE_ADD_PAYMENT (posman_add_payment_get_type())

G_DECLARE_FINAL_TYPE (PosmanAddPayment, posman_add_payment, POSMAN, ADD_PAYMENT, GtkBox)

PosmanAddPayment *posman_add_payment_new (void);
void
posman_add_payment_set_cust_id(PosmanAddPayment   *add_pay,gint  cust_id);
gint
posman_add_payment_get_cust_id (PosmanAddPayment   *self);
GtkWidget *
posman_add_payment_get_model (PosmanAddPayment   *self);

G_END_DECLS
