#include "posman-list-box-data.h"

void
row_cust_data_free(RowCustData *row)
{
  free(row->id);
  free(row->name);
  free(row);
}

GtkWidget *
row_cust_data_create(gpointer item,
                     gpointer user_data)
{
  GtkWidget     *row = GTK_WIDGET (item);
  RowCustData   *data = g_object_get_data(G_OBJECT(row),"data");

  GtkWidget   *grid,*label;

  grid = g_object_new (GTK_TYPE_GRID,
                       "visible", TRUE,
                       "hexpand", TRUE,
                       "border-width", 12,
                       "column-spacing", 12,
                       NULL);

  label = g_object_new (GTK_TYPE_LABEL,
                        "label", data->name,
                        "visible", TRUE,
                        "xalign", 0.0,
                        "hexpand", TRUE,
                        NULL);

  gtk_grid_attach (GTK_GRID (grid), label, 0, 0, 1, 1);
  gtk_container_add (GTK_CONTAINER (row), grid);
  gtk_widget_show (row);

  return row;
}

gpointer
row_cust_data_new(const gchar *id,
                  const gchar *name)
{
  RowCustData *row = g_new0 (RowCustData, 1);
  row->row  = gtk_list_box_row_new ();
  row->id   = g_strdup (id);
  row->name = g_strdup (name);

  g_object_set_data_full (G_OBJECT (row->row), "data", row, (GDestroyNotify) row_cust_data_free);

  return (gpointer) row->row;
}

void
row_cmnd_data_free(RowCmndData *row)
{
  free(row->id);
  free(row->state);
  free(row->date);
  free(row->total);
  free(row);
}
GtkWidget *
row_cmnd_data_create(gpointer item,
                     gpointer user_data)
{
  GtkWidget   *grid,*label;

  GtkWidget     *row = GTK_WIDGET (item);
  RowCmndData   *data = g_object_get_data(G_OBJECT(row),"data");

  grid = g_object_new (GTK_TYPE_GRID,
                       "visible", TRUE,
                       "hexpand", TRUE,
                       "border-width", 12,
                       "column-spacing", 12,
                       NULL);

  label = g_object_new (GTK_TYPE_LABEL,
                        "label", data->date,
                        "visible", TRUE,
                        "xalign", 0.0,
                        "hexpand", TRUE,
                        NULL);

  gtk_grid_attach (GTK_GRID (grid), label, 0, 0, 1, 1);
  gtk_container_add (GTK_CONTAINER (row), grid);
  gtk_widget_show (row);

  return row;
}

gpointer
row_cmnd_data_new(const gchar *id,
                  const gchar *state,
                  const gchar *date,
                  const gchar *total)
{
  RowCmndData *row    = g_new0 (RowCmndData, 1);

  row->row  = gtk_list_box_row_new ();
  row->id   = g_strdup (id);
  row->state = g_strdup (state);
  row->date = g_strdup (date);
  row->total = g_strdup (total);


  g_object_set_data_full (G_OBJECT (row->row), "data", row, (GDestroyNotify) row_cmnd_data_free);

  return row->row;
}
