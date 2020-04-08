#include <gtk/gtk.h>
#include "posman-panel-list-row.h"

static void
row_cust_data_free (CustRowData *data)
{
  g_free (data);
}

static void
row_cmnd_data_free (CustRowData *data)
{
  g_free (data);
}


CustRowData*
row_cust_data_new (const int            id,
                   const int            domain,
                   const gchar         *name,
                   const gdouble        rest)
{
  GtkWidget *label, *grid;
  CustRowData *data;

  data = g_new0 (CustRowData, 1);

  data->row = gtk_list_box_row_new ();
  data->cust_id = id;
  data->rest =  rest;
  data->cust_domain = domain;


  /* Setup the row */
  grid = g_object_new (GTK_TYPE_GRID,
                       "visible", TRUE,
                       "hexpand", TRUE,
                       "border-width", 12,
                       "column-spacing", 12,
                       NULL);


  /* Name label */
  data->cust_name = g_object_new (GTK_TYPE_LABEL,
                                  "label", name,
                                  "visible", TRUE,
                                  "xalign", 0.0,
                                  "hexpand", TRUE,
                                  NULL);
  gtk_grid_attach (GTK_GRID (grid), data->cust_name, 0, 0, 1, 1);

  /* Description label */
  label = g_object_new (GTK_TYPE_LABEL,
                        "label", "<small>rest :</small>",
                        "visible", FALSE,
                        "xalign", 0.0,
                        "use-markup",TRUE,
                        "hexpand", TRUE,
                        NULL);

  gtk_style_context_add_class (gtk_widget_get_style_context (label), "dim-label");
  gtk_grid_attach (GTK_GRID (grid), label, 0, 1, 1, 1);

  gchar buf[G_ASCII_DTOSTR_BUF_SIZE];
  data->cust_rest = g_object_new (GTK_TYPE_LABEL,
                                  "label",g_ascii_formatd(buf,
                                                          G_ASCII_DTOSTR_BUF_SIZE,
                                                          "<small>%.2f</small>",rest),
                                  "visible", FALSE,
                                  "xalign", 1.0,
                                  "use-markup",TRUE,
                                  "hexpand", TRUE,
                                  NULL);

  gtk_style_context_add_class (gtk_widget_get_style_context (data->cust_rest), "dim-label");
  gtk_grid_attach (GTK_GRID (grid), data->cust_rest, 1, 1, 1, 1);

  gtk_container_add (GTK_CONTAINER (data->row), grid);
  gtk_widget_show_all (data->row);

  g_object_set_data_full (G_OBJECT (data->row), "data", data, (GDestroyNotify) row_cust_data_free);

  return data;
}

CmndRowData*
row_cmnd_data_new (const int            id,
                   const gchar         *name,
                   const gdouble        rest)
{
  GtkWidget   *label, *grid;
  CmndRowData *data;

  data = g_new0 (CmndRowData, 1);

  data->row = gtk_list_box_row_new ();
  data->cmnd_id = id;
  data->rest =  rest;


  /* Setup the row */
  grid = g_object_new (GTK_TYPE_GRID,
                       "visible", TRUE,
                       "hexpand", TRUE,
                       "border-width", 12,
                       "column-spacing", 12,
                       "no-show-all",TRUE,
                       NULL);


  /* Name label */
  data->cmnd_name = g_object_new (GTK_TYPE_LABEL,
                                  "label", name,
                                  "visible", TRUE,
                                  "xalign", 0.0,
                                  "hexpand", TRUE,
                                  NULL);
  gtk_grid_attach (GTK_GRID (grid), data->cmnd_name, 0, 0, 1, 1);

  /* Description label */
  label = g_object_new (GTK_TYPE_LABEL,
                        "label", "<small>rest :</small>",
                        "visible", TRUE,
                        "xalign", 0.0,
                        "use-markup",TRUE,
                        "hexpand", TRUE,
                        NULL);

  gtk_style_context_add_class (gtk_widget_get_style_context (label), "dim-label");
  gtk_grid_attach (GTK_GRID (grid), label, 0, 1, 1, 1);

  gchar buf[G_ASCII_DTOSTR_BUF_SIZE];
  data->cmnd_rest = g_object_new (GTK_TYPE_LABEL,
                                  "label",g_ascii_formatd(buf,
                                                          G_ASCII_DTOSTR_BUF_SIZE,
                                                          "<small>%.2f</small>",rest),
                                  "visible", TRUE,
                                  "xalign", 1.0,
                                  "use-markup",TRUE,
                                  "hexpand", TRUE,
                                  NULL);

  gtk_style_context_add_class (gtk_widget_get_style_context (data->cmnd_rest), "dim-label");
  gtk_grid_attach (GTK_GRID (grid), data->cmnd_rest, 1, 1, 1, 1);

  data->in_add = g_object_new (GTK_TYPE_IMAGE,
                               "icon-name","document-edit-symbolic",
                               "visible", FALSE,
                               "xalign", 1.0,
                               NULL);

  gtk_grid_attach (GTK_GRID (grid), data->in_add, 1, 0, 1, 1);

  gtk_container_add (GTK_CONTAINER (data->row), grid);
  gtk_widget_show_all (data->row);

  g_object_set_data_full (G_OBJECT (data->row), "data", data, (GDestroyNotify) row_cmnd_data_free);

  return data;
}
