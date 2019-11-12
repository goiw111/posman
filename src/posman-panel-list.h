#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>
#include <sqlite3.h>
G_BEGIN_DECLS

typedef enum{
  posman_panel_list_main,
  posman_panel_list_cust
} posmanpanellistview;

#define POSMAN_TYPE_PANEL_LIST (posman_panel_list_get_type())

G_DECLARE_FINAL_TYPE (PosmanPanelList,
                      posman_panel_list,
                      POSMAN, PANEL_LIST,
                      GtkStack)

PosmanPanelList       *posman_panel_list_new (void);
void                  posman_panel_list_main_model_init(PosmanPanelList *self,
                                       sqlite3 *db);
posmanpanellistview   posman_panel_list_get_view(PosmanPanelList *self);
void                  postman_panel_list_set_view(PosmanPanelList *self,
                                                  posmanpanellistview view);

G_END_DECLS