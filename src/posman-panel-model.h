#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

enum
{
  COL_ID,
  COL_NAME,

  N_COL
};

typedef enum
{
  posmanpanelmodelmain,
  posmanpanelmodelcust
}posmanpanelmodeltype;

#define POSMAN_TYPE_PANEL_MODEL (posman_panel_model_get_type())

G_DECLARE_FINAL_TYPE (PosmanPanelModel,
                      posman_panel_model,
                      POSMAN, PANEL_MODEL,
                      GtkListStore)

PosmanPanelModel *posman_panel_model_new (void);

void
posman_panel_model_add_cust(PosmanPanelModel *self, const unsigned char *id, const unsigned char *name);

void
posman_panel_model_main_init(PosmanPanelModel *self);

G_END_DECLS
