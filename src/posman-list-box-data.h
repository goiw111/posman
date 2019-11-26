#pragma once

#include <gtk/gtk.h>

typedef struct {
  GtkWidget                 *row;

  gchar                     *id;
  gchar                     *name;

}RowCustData;

typedef struct {
  GtkWidget                 *row;

  gchar                     *id;
  gchar                     *state;
  gchar                     *date;
  gchar                     *total;

}RowCmndData;

void
row_cust_data_free(RowCustData *row);
GtkWidget *
row_cust_data_create(gpointer item,
                     gpointer user_data);
gpointer
row_cust_data_new(const gchar *id,
                  const gchar *name);


void
row_cmnd_data_free(RowCmndData *row);
GtkWidget *
row_cmnd_data_create(gpointer item,
                     gpointer user_data);
gpointer
row_cmnd_data_new(const gchar *id,
                  const gchar *state,
                  const gchar *date,
                  const gchar *total);
