
#pragma once

#include <gtk/gtk.h>
#include <sqlite3.h>

G_BEGIN_DECLS

#define POSMAN_TYPE_WINDOW (posman_window_get_type())

G_DECLARE_FINAL_TYPE (PosmanWindow, posman_window, POSMAN, WINDOW, GtkApplicationWindow)

void
posman_window_remove_cust_with_id(PosmanWindow *self, gint64  id);


GtkWidget *
posman_window_get_action_menu_cust(PosmanWindow *self);
GtkWidget *
posman_window_get_select_button(PosmanWindow *self);
GtkWidget *
posman_window_get_remove_button(PosmanWindow *self);
GtkWidget *
posman_window_get_panel_list(PosmanWindow *self);
sqlite3 *
posman_window_get_db (PosmanWindow *self);

G_END_DECLS
