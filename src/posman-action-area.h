#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define POSMAN_TYPE_ACTION_AREA (posman_action_area_get_type())

G_DECLARE_FINAL_TYPE (PosmanActionArea, posman_action_area, POSMAN, ACTION_AREA, GtkStack)

typedef enum
{
  posman_action_area_view_main,
  posman_action_area_view_add_cmnd,
  posman_action_area_view_payment_validation
}PosmanActionAreaView;

PosmanActionArea *posman_action_area_new (void);

void
posman_action_area_set_view(PosmanActionArea      *self,
                            PosmanActionAreaView  view);
PosmanActionAreaView
posman_action_area_get_view(PosmanActionArea  *self);
void
posman_action_area_set_items(PosmanActionArea *self,
                             GtkListStore     *model);
GtkWidget *
posman_action_area_get_items(PosmanActionArea *self);
void
posman_action_area_set_items_view(PosmanActionArea  *self,
                                  GtkListStore      *model);
GtkWidget *
posman_action_area_get_items_viewer(PosmanActionArea  *self);
void
posman_action_area_set_cust_id(PosmanActionArea  *self,
                               gint64            cust_id);
gint64
posman_action_area_get_cust_id(PosmanActionArea *self);

gint
posman_action_area_get_cust_dmn(PosmanActionArea *self);

void
posman_action_area_set_cust_dmn(PosmanActionArea  *self,
                                gint            cust_dmn);

GtkIconView *
posman_action_area_get_iconview_item_viewer(PosmanActionArea  *self);

void
posman_action_area_set_qt_adj(PosmanActionArea  *self,GtkAdjustment *adj);
void
posman_action_area_set_price_adj(PosmanActionArea  *self,GtkAdjustment *adj);

GtkComboBox*
posman_action_area_get_combo(PosmanActionArea  *self);

void
posman_action_area_set_visible(PosmanActionArea  *self,
                               gboolean          visible);

G_END_DECLS
