
#pragma once

#include <gtk/gtk.h>
#include "posman-add-cmnd.h"

G_BEGIN_DECLS

#define POSMAN_TYPE_WINDOW (posman_window_get_type())

G_DECLARE_FINAL_TYPE (PosmanWindow, posman_window, POSMAN, WINDOW, GtkApplicationWindow)


G_END_DECLS
