#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef  enum{
  posman_type_item_item,
  posman_type_item_stock,
  posman_type_item_category
}posmanitemtype;

#define POSMAN_TYPE_ITEM (posman_item_get_type())

G_DECLARE_FINAL_TYPE (PosmanItem, posman_item, POSMAN, ITEM, GtkFlowBoxChild)

PosmanItem *posman_item_new (const  gchar     *name,
                             const  gchar     *image,
                             const  gint       cust_id);
PosmanItem *posman_item_new_category (const  gchar     *name,
                                      const  gchar     *image,
                                      const  gint       cust_id);
PosmanItem *posman_item_new_stock(const  gchar     *name,
                                  const  gchar     *image,
                                  const  gfloat     price,
                                  const  gint       quantity,
                                  const  gint       cust_id);
void
posman_item_set_item_type (PosmanItem    *self,posmanitemtype  type);
posmanitemtype
posman_item_get_item_type (PosmanItem    *self);
void
posman_item_set_image(PosmanItem    *self,const gchar    *Resource);
void
posman_item_set_price(PosmanItem    *self,gfloat    price);
void
posman_item_set_name (PosmanItem    *self,const gchar    *name);
void
posman_item_set_selected(PosmanItem *self,gboolean  selected);
void
posman_item_set_quantity(PosmanItem *self,gint      quantity);
void
posman_item_set_id(PosmanItem *self,gint  item_id);
int
posman_item_get_id(PosmanItem *self);

G_END_DECLS
