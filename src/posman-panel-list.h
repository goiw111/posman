#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>
#include <sqlite3.h>
#include "posman-action-menu-cust.h"
#include "posman-action-menu-cmnd.h"
#include "posman-panel-row-cust.h"
G_BEGIN_DECLS

typedef enum{
  posman_panel_list_main,
  posman_panel_list_cust,
  posman_panel_list_add_cust
} posmanpanellistview;

#define POSMAN_TYPE_PANEL_LIST (posman_panel_list_get_type())

G_DECLARE_FINAL_TYPE (PosmanPanelList,
                      posman_panel_list,
                      POSMAN, PANEL_LIST,
                      GtkStack)

PosmanPanelList
*posman_panel_list_new (void);

posmanpanellistview
posman_panel_list_get_view(PosmanPanelList *self);
void
posman_panel_list_set_view(PosmanPanelList *self,
                           posmanpanellistview view);

void
posman_panel_list_remove_row_cust(PosmanPanelList     *self,
                                  PosmanPanelRowCust  *row);

void
posman_panel_list_set_list_stor_cust(PosmanPanelList *self,
                                     GObject         *list_stor);

gboolean
posman_panel_list_is_add_cust_in_prog(PosmanPanelList *self);
void
posman_panel_list_clear_add_cust(PosmanPanelList *self);

G_END_DECLS
