#pragma once

#include <glib-object.h>
#include <sqlite3.h>

G_BEGIN_DECLS

#define POSMAN_TYPE_PANEL_LOADER (posman_panel_loader_get_type ())

G_DECLARE_INTERFACE (PosmanPanelLoader, posman_panel_loader, POSMAN, PANEL_LOADER, GObject)

struct _PosmanPanelLoaderInterface
{
  GTypeInterface parent;

  gboolean (*get_cust) (PosmanPanelLoader *self, sqlite3 *db);
};

gboolean posman_panel_loader_get_cust(PosmanPanelLoader *self, sqlite3 *db);

G_END_DECLS
