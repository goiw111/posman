#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define POSMAN_TYPE_ACTION_CONTROLER (posman_action_controler_get_type())

G_DECLARE_FINAL_TYPE (PosmanActionControler, posman_action_controler, POSMAN, ACTION_CONTROLER, GtkListBoxRow)

PosmanActionControler *posman_action_controler_new (const gchar   *name,gint  cmnd_id);
const gchar *
posman_action_controler_get_name(PosmanActionControler *self);

void
posman_action_controler_set_cmnd_id(PosmanActionControler *self,gint    cmnd_id);
gint
posman_action_controler_get_cmnd_id(PosmanActionControler *self);

G_END_DECLS
