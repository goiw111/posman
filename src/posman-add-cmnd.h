#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>
#include "posman-item-list-row.h"

G_BEGIN_DECLS

#define POSMAN_TYPE_ADD_CMND (posman_add_cmnd_get_type())

G_DECLARE_FINAL_TYPE (PosmanAddCmnd, posman_add_cmnd, POSMAN, ADD_CMND, GtkBox)

PosmanAddCmnd *
posman_add_cmnd_new (gint   cmnd_id,gint    cmnd_domain);
GtkFlowBox *
posman_add_cmnd_get_item_flow(PosmanAddCmnd   *self);
GtkFlowBox *
posman_add_cmnd_get_stock_flow(PosmanAddCmnd   *self);
GtkListBox *
posman_add_cmnd_get_selected_list(PosmanAddCmnd   *self);
void
posman_add_cmnd_set_cmnd_id(PosmanAddCmnd   *self,gint  cmnd_id);
gint
posman_add_cmnd_get_cmnd_id(PosmanAddCmnd   *self);
void
posman_add_cmnd_set_cmnd_domain(PosmanAddCmnd   *self,gint  cmnd_domain);
gint
posman_add_cmnd_get_cmnd_domain(PosmanAddCmnd   *self);
void
posman_add_cmnd_load(PosmanAddCmnd    *self);
const gchar *
posman_add_cmnd_get_cmnd_name(PosmanAddCmnd    *self);
void
posman_add_cmnd_set_cmnd_name(PosmanAddCmnd    *self,gchar    *name);
void
posman_add_cmnd_set_stock_visible(PosmanAddCmnd    *self,gboolean   visible);
void
posman_add_cmnd_add_item (PosmanAddCmnd    *self,PosmanItemListRow    *row);
PosmanItemListRow *
posman_add_cmnd_get_list_row (PosmanAddCmnd    *self,gint   row_id);

G_END_DECLS
