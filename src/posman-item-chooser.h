#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>
#include "posman-item.h"

G_BEGIN_DECLS

#define POSMAN_TYPE_ITEM_CHOOSER (posman_item_chooser_get_type())

G_DECLARE_FINAL_TYPE (PosmanItemChooser, posman_item_chooser, POSMAN, ITEM_CHOOSER, GtkOverlay)

PosmanItemChooser *posman_item_chooser_new (void);


GtkFlowBox *
posman_item_chooser_get_items_flow(PosmanItemChooser   *self);

GtkFlowBox *
posman_item_chooser_get_stock_flow(PosmanItemChooser   *self);
void
posman_item_chooser_set_stock_visible(PosmanItemChooser   *self,gboolean  visible);

G_END_DECLS
