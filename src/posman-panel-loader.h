#pragma once

#include <glib-object.h>

G_BEGIN_DECLS

#define POSMAN_TYPE_PANEL_LOADER (posman_panel_loader_get_type ())

G_DECLARE_INTERFACE (PosmanPanelLoader, posman_panel_loader, POSMAN, PANEL_LOADER, GObject)

struct _PosmanPanelLoaderInterface
{
  GTypeInterface parent;

  void (*add_items) (PosmanPanelLoader *self);
};

G_END_DECLS
