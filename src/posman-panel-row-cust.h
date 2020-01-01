#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>
#include "posman-action-area.h"

G_BEGIN_DECLS

#define POSMAN_TYPE_PANEL_ROW_CUST (posman_panel_row_cust_get_type())

G_DECLARE_FINAL_TYPE (PosmanPanelRowCust, posman_panel_row_cust, POSMAN, PANEL_ROW_CUST, GtkListBoxRow)

PosmanPanelRowCust *posman_panel_row_cust_new (void);

void
posman_panel_row_cust_set_name(PosmanPanelRowCust *self,const gchar *name);
void
posman_panel_row_cust_set_id(PosmanPanelRowCust *self,gint64 id);
gint64
posman_panel_row_cust_get_id(PosmanPanelRowCust *self);
PosmanActionArea *
posman_panel_row_cust_get_action_area(PosmanPanelRowCust *self);
void
posman_panel_row_cust_set_action_area(PosmanPanelRowCust *self,
                                      PosmanActionArea   *action_area);
G_END_DECLS
