#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>
#include <sqlite3.h>
G_BEGIN_DECLS

typedef enum{
  posman_panel_list_main,
  posman_panel_list_cust,
  posman_panel_list_add_cust
} posmanpanellistview;

#define POSMAN_TYPE_PANEL_LIST (posman_panel_list_get_type())

G_DECLARE_FINAL_TYPE (PosmanPanelList,
                      posman_panel_list,
                      POSMAN, PANEL_LIST,
                      GtkStack)

PosmanPanelList
*posman_panel_list_new (void);

posmanpanellistview
posman_panel_list_get_view(PosmanPanelList *self);
void
posman_panel_list_set_view(PosmanPanelList *self,
                           posmanpanellistview view);

void
posman_panel_list_set_model_cust(PosmanPanelList *self,
                                 GObject         *list_stor);
void
posman_panel_list_set_model_cmnd(PosmanPanelList *self,
                                 GObject         *list_stor);
void
posman_panel_list_set_model_domain(PosmanPanelList  *self,
                                   GtkListStore     *list_stor);

const gchar *
posman_panel_list_get_name_entry_text(PosmanPanelList  *self);
const gchar *
posman_panel_list_get_adress_entry_text(PosmanPanelList  *self);
const gchar *
posman_panel_list_get_phone_entry_text(PosmanPanelList  *self);
const gchar *
posman_panel_list_get_domain_combobox_id(PosmanPanelList  *self);
gchar *
posman_panel_list_get_description_textview_text(PosmanPanelList  *self);
G_END_DECLS
