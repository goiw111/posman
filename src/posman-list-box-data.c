#include "posman-list-box-data.h"
#include "posman-window.h"
#include <sqlite3.h>

typedef struct {
  PosmanWindow  *win;
  RowCustData   *data;
  GtkWidget     *menu;
  GtkWidget     *slct;
  gulong        handler_id_menu;
  gulong        handler_id_slct;
} reamovedata;

void
row_cust_data_free(RowCustData *row)
{
  free(row->id);
  free(row->name);
  free(row);
}

static void
row_cust_data_remove(GtkButton      *button,
                     GtkCheckButton *check)
{
  reamovedata *data = g_object_get_data (G_OBJECT(check),"data");
  g_signal_handler_disconnect(data->slct,data->handler_id_slct);
  g_signal_handler_disconnect(data->menu,data->handler_id_menu);
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (check)) == TRUE)
    {
      int         rc;
      g_autofree gchar *sql;
      char             *err_msg = 0;

      sql = g_strdup_printf ("DELETE FROM customer WHERE id = %s;",data->data->id);
      rc = sqlite3_exec (posman_window_get_db (data->win),sql,0,0,&err_msg);
      if (rc != SQLITE_OK)
        {
      g_warning ("Failed to execute statement: %s\n", err_msg);
      return;
        }

    }
}

GtkWidget *
row_cust_data_create(gpointer item,
                     gpointer user_data)
{
  GtkWidget     *row = GTK_WIDGET (item);
  GtkWidget     *win = GTK_WIDGET (user_data);
  RowCustData   *data = g_object_get_data(G_OBJECT(row),"data");

  GtkWidget   *grid,*label,*check;
  reamovedata *rdata = g_new (reamovedata, 1);
  rdata->menu = posman_window_get_action_menu(POSMAN_WINDOW(win));
  rdata->slct = posman_window_get_select_button(POSMAN_WINDOW (win));
  rdata->data = data;
  rdata->win  = POSMAN_WINDOW (win);

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

  check = g_object_new (GTK_TYPE_CHECK_BUTTON,
                        NULL);
  g_object_set_data(G_OBJECT (check),"data",rdata);

  rdata->handler_id_menu =
  g_signal_connect_swapped (rdata->menu, "remove_pressed",
                            G_CALLBACK (gtk_widget_show),
                            check);
  rdata->handler_id_slct =
  g_signal_connect (rdata->slct,"clicked",
                    G_CALLBACK (row_cust_data_remove),
                    check);

  gtk_grid_attach (GTK_GRID (grid), label, 0, 0, 1, 1);
  gtk_grid_attach (GTK_GRID (grid), check, 1, 0, 1, 1);
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
