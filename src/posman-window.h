
#pragma once

#include <gtk/gtk.h>
#include <sqlite3.h>

G_BEGIN_DECLS

#define POSMAN_TYPE_WINDOW (posman_window_get_type())

G_DECLARE_FINAL_TYPE (PosmanWindow, posman_window, POSMAN, WINDOW, GtkApplicationWindow)

GtkWidget *
posman_window_get_action_menu(PosmanWindow *self);
GtkWidget *
posman_window_get_select_button(PosmanWindow *self);
sqlite3 *
posman_window_get_db (PosmanWindow *self);

G_END_DECLS
