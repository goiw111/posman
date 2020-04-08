#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define POSMAN_TYPE_ITEM_LIST_ROW (posman_item_list_row_get_type())

G_DECLARE_FINAL_TYPE (PosmanItemListRow, posman_item_list_row, POSMAN, ITEM_LIST_ROW, GtkListBoxRow)

PosmanItemListRow *
posman_item_list_row_new (gint            item_id,
                          GtkAdjustment  *adjustment_price,
                          GtkAdjustment  *adjustment_qt,
                          gint            qt_pack_unit,
                          gchar          *name,
                          gint            id);
gint
posman_item_list_get_id(PosmanItemListRow *self);
void
posman_item_list_set_id(PosmanItemListRow    *self,gint  item_id);
gint
posman_item_list_get_in_order_id(PosmanItemListRow *self);
void
posman_item_list_set_in_order_id(PosmanItemListRow    *self,gint  in_order_id);
void
posman_item_list_set_price(PosmanItemListRow    *self,GtkAdjustment   *adjustment);
GtkWidget *
posman_item_list_get_price(PosmanItemListRow    *self);
void
posman_item_list_set_qt(PosmanItemListRow    *self,GtkAdjustment   *adjustment);
GtkWidget *
posman_item_list_get_qt(PosmanItemListRow    *self);
void
posman_item_list_set_qt_pack(PosmanItemListRow    *self,gint   qt_pack_unit);
gint
posman_item_list_get_qt_pack(PosmanItemListRow    *self);

G_END_DECLS
