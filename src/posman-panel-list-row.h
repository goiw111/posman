#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct
{
  GtkWidget          *row;
  GtkWidget          *cust_name;
  GtkWidget          *cust_rest;
  gint                cust_id;
  gint                cust_domain;
  gdouble             rest;

} CustRowData;

typedef struct
{
  GtkWidget          *row;
  GtkWidget          *cmnd_name;
  GtkWidget          *cmnd_rest;
  GtkWidget          *add_cmnd;
  GtkWidget          *in_add;
  gint                cmnd_id;
  gdouble             rest;

} CmndRowData;

CustRowData*
row_cust_data_new (const gint           id,
                   const gint           domain,
                   const gchar         *name,
                   const gdouble        rest);

CmndRowData*
row_cmnd_data_new (const int            id,
                   const gchar         *name,
                   const gdouble        rest);

G_END_DECLS
