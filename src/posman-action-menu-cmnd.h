#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define POSMAN_TYPE_ACTION_MENU_CMND (posman_action_menu_cmnd_get_type())

G_DECLARE_FINAL_TYPE (PosmanActionMenuCmnd, posman_action_menu_cmnd, POSMAN, ACTION_MENU_CMND, GtkMenuButton)

PosmanActionMenuCmnd *posman_action_menu_cmnd_new (void);

G_END_DECLS
