#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>
G_BEGIN_DECLS

typedef enum{
  posman_panel_list_main,
  posman_panel_list_cust,
  posman_panel_list_add_cust
} posmanpanellistview;

#define POSMAN_TYPE_PANEL_LIST (posman_panel_list_get_type())

G_DECLARE_FINAL_TYPE (PosmanPanelList,posman_panel_list,POSMAN, PANEL_LIST,GtkStack)

PosmanPanelList*
posman_panel_list_new (void);


void
posman_panel_list_clear(PosmanPanelList *self);
void
posman_panel_list_set_view(PosmanPanelList *self,
                           posmanpanellistview view);
posmanpanellistview
posman_panel_list_get_view(PosmanPanelList  *self);
void
posman_panel_list_set_cust_id(PosmanPanelList  *self,
                              int  cust_id);
int
posman_panel_list_get_cust_id(PosmanPanelList  *self);
gint
posman_panel_list_get_cust_domain(PosmanPanelList   *self);
void
posman_panel_list_set_cust_domain(PosmanPanelList   *self,
                                  gint    cust_domain);
void
posman_panel_list_set_init_mode(PosmanPanelList *self,
                                gboolean         init_mode);
GtkListStore *
posman_panel_list_get_cust_model(PosmanPanelList  *self);
GtkListStore *
posman_panel_list_get_domn_model(PosmanPanelList  *self);

gboolean
posman_panel_list_check_data(PosmanPanelList  *self,
                             GtkWidget        *content_label);
void
posman_panel_list_insert_cust(PosmanPanelList   *self,
                              GtkWidget         *cust_row);
void
posman_panel_list_insert_cmnd(PosmanPanelList   *self,
                              GtkWidget         *cmnd_row);
void
posman_panel_list_remove_cmnd(PosmanPanelList   *self,
                              GtkWidget         *cmnd_row);
void
posman_panel_list_load_edit_cust(PosmanPanelList   *self,
                                 GtkListBoxRow     *row,
                                 gchar             *name,
                                 gchar             *address,
                                 gchar             *domain_id,
                                 gchar             *description,
                                 gchar             *phone);
GtkListBoxRow  *
posman_panel_list_get_selected_cust(PosmanPanelList   *self);
GtkListBoxRow  *
posman_panel_list_get_selected_cmnd(PosmanPanelList   *self);
void
posman_panel_list_select_cmnd(PosmanPanelList   *self,GtkListBoxRow   *row);
GtkWidget *
posman_panel_list_get_cmnd_row(PosmanPanelList   *self,gint   cmnd_id);
void
posman_panel_list_reload_cust(PosmanPanelList   *self);
void
posman_panel_list_clear_cust(PosmanPanelList   *self);

G_END_DECLS
