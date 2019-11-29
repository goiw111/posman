#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define POSMAN_TYPE_ACTION_MENU (posman_action_menu_get_type())

G_DECLARE_FINAL_TYPE (PosmanActionMenu, posman_action_menu, POSMAN, ACTION_MENU, GtkMenuButton)

PosmanActionMenu *posman_action_menu_new (void);

G_END_DECLS
