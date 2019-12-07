#pragma once

#include <gtk/gtk.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define POSMAN_TYPE_ACTION_MENU_CUST (posman_action_menu_cust_get_type())

G_DECLARE_FINAL_TYPE (PosmanActionMenuCust, posman_action_menu_cust, POSMAN, ACTION_MENU_CUST, GtkMenuButton)

PosmanActionMenuCust *posman_action_menu_cust_new (void);
void
posman_action_menu_cust_set_remove_button(PosmanActionMenuCust *self,
                                          GtkButton            *button);

G_END_DECLS
