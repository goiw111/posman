
#include "posman-config.h"
#include "posman-window.h"
#include "posman-panel-list.h"
#include "posman-action-menu.h"
#include "posman-list-box-data.h"
#include <sqlite3.h>

struct _PosmanWindow
{
  GtkApplicationWindow  parent_instance;

  /* Template widgets */
  GtkWidget             *previous_button;
  GtkWidget             *panel_list;
  GtkWidget             *action_menu;
  GtkWidget             *select_button;


  /* database */
  sqlite3               *db;


};

enum
{
  COL_CUST_ID,
  COL_CUST_NAME,

  N_CUST_COL
};

enum
{
  COL_CMND_ID,
  COL_CMND_STATE,
  COL_CMND_DATE,
  COL_CMND_TOTAL,

  N_CMND_COL
};

G_DEFINE_TYPE (PosmanWindow, posman_window, GTK_TYPE_APPLICATION_WINDOW)

static void
posman_window_update_cust(PosmanWindow *self);

static void
posman_window_init_database(PosmanWindow *self)
{
  g_autofree char *dir = NULL;

  dir = g_strdup_printf("%s/""posman.db",g_get_user_data_dir());

  int rc = sqlite3_open(dir, &(self->db));

  if (rc != SQLITE_OK)
    {
      g_warning ("Cannot open database: %s\n", sqlite3_errmsg(self->db));
      sqlite3_close (self->db);
    }
}

static GPtrArray *
get_custs(PosmanWindow *self)
{
  sqlite3_stmt *stmt;
  int          rc;
  GPtrArray    *array;

  rc = sqlite3_prepare_v2(self->db,"SELECT id, full_name FROM customer;",-1,
                          &stmt,NULL);
  if (rc != SQLITE_OK)
    {
      g_warning ("Failed to execute statement: %s\n", sqlite3_errmsg(self->db));
      return NULL;
    }

  array = g_ptr_array_new_full (0, g_object_unref);

  while(sqlite3_step(stmt) != SQLITE_DONE)
    {
      g_ptr_array_add (array,
                       row_cust_data_new (sqlite3_column_text (stmt, COL_CUST_ID),
                                          sqlite3_column_text (stmt, COL_CUST_NAME)));
    }

  g_print("");
  sqlite3_finalize(stmt);
  return array;
}

static GPtrArray *
get_cmnds(PosmanWindow *self,
          gchar        *cust_id)
{
  sqlite3_stmt *stmt;
  int          rc;
  GPtrArray    *array;
  g_autofree gchar    *sql = NULL;


  sql = g_strdup_printf("select id,status,order_date from orders where cust_id = %s;",cust_id);
  rc = sqlite3_prepare_v2(self->db,sql,-1,
                          &stmt,NULL);
  if (rc != SQLITE_OK)
    {
      g_warning ("Failed to execute statement: %s\n", sqlite3_errmsg(self->db));
      return NULL;
    }
  array = g_ptr_array_new_full (0, g_object_unref);

  while(sqlite3_step(stmt) != SQLITE_DONE)
    g_ptr_array_add (array,
                     row_cmnd_data_new (sqlite3_column_text (stmt, COL_CMND_ID),
                                        sqlite3_column_text (stmt, COL_CMND_STATE),
                                        sqlite3_column_text (stmt, COL_CMND_DATE),
                                        (const char*)"NULL"));


  sqlite3_finalize(stmt);
  return array;
}

static GtkListStore*
get_domains(PosmanWindow  *self)
{
  sqlite3_stmt  *stmt;
  int           rc;
  GtkListStore  *list;

  list = gtk_list_store_new(2,G_TYPE_STRING,G_TYPE_STRING);
  rc = sqlite3_prepare_v2(self->db,"SELECT id, name FROM domains;",-1,
                          &stmt,NULL);
  if (rc != SQLITE_OK)
    {
      g_warning ("Failed to execute statement: %s\n", sqlite3_errmsg(self->db));
      return NULL;
    }

  while(sqlite3_step(stmt) != SQLITE_DONE)
    gtk_list_store_insert_with_values(list,NULL,-1,
                                      0,sqlite3_column_text (stmt, 0),
                                      1,sqlite3_column_text (stmt, 1),
                                      -1);

  sqlite3_finalize(stmt);
  return list;
}
/* callback */

static void
add_pressed_cb(PosmanActionMenu *menu,
               PosmanWindow     *self)
{
  PosmanPanelList   *list;
  GtkListStore      *stor;

  list = POSMAN_PANEL_LIST (self->panel_list);
  stor = get_domains(self);

  posman_panel_list_set_model_domain (list, stor);
  g_object_unref (stor);

  posman_panel_list_set_view (POSMAN_PANEL_LIST (self->panel_list), posman_panel_list_add_cust);

  gtk_widget_set_visible(GTK_WIDGET (menu),FALSE);
  gtk_widget_set_visible(GTK_WIDGET (self->select_button),TRUE);
}

static void
select_pressed_cb(GtkButton      *button,
                  PosmanWindow   *self)
{
  PosmanPanelList       *list = POSMAN_PANEL_LIST (self->panel_list);
  const gchar           *name,*phone,*adress,*domain_id;
  gchar                 *description;

  int                   rc;
  char                  *err_msg = 0;
  g_autofree gchar      *sql = NULL;

  name = posman_panel_list_get_name_entry_text (list);
  /*if(name == NULL || !g_strcmp0(name,"") || g_strv_length((gchar**)&name) <= 3)
    return;*/
  phone = posman_panel_list_get_phone_entry_text (list);
  /*if(phone == NULL || !g_strcmp0(phone,"") || g_strv_length((gchar**)&phone) <= 10)
    return;*/
  adress = posman_panel_list_get_adress_entry_text (list);
  /*if(adress == NULL || !g_strcmp0(adress,"") || g_strv_length((gchar**)&adress) <= 3)
    return;*/
  domain_id = posman_panel_list_get_domain_combobox_id (list);
  /*if(domain_id == NULL || !g_strcmp0(domain_id,""))
    return;*/
  description = posman_panel_list_get_description_textview_text (list);
  /*if(description == NULL || !g_strcmp0(description,"") || g_strv_length((gchar**)&description) < 5)
    return;*/

  g_print("here");

  sql = g_strdup_printf("INSERT INTO customer "
                        "(full_name,address,domain_id,description,phone)"
                        " VALUES (' %s',' %s',%s,' %s',' %s');",
                        name,adress,domain_id,
                        description,phone);

  rc = sqlite3_exec (self->db,sql,0,0,&err_msg);
  if (rc != SQLITE_OK)
    {
      g_warning ("Failed to execute statement: %s\n", err_msg);
      return;
    }

  posman_window_update_cust(self);

  g_free(description);
}

static void
panel_list_view_changed_cb (PosmanPanelList *panel_list,
                                 GParamSpec  *pspec,
                                 PosmanWindow    *self)
{
  gboolean is_main_view;

  is_main_view = posman_panel_list_get_view (panel_list) == posman_panel_list_main;

  gtk_widget_set_visible (self->previous_button, !is_main_view);
}

static void
previous_button_clicked_cb(GtkButton *button,
                           PosmanWindow    *self)
{
  posman_panel_list_set_view (POSMAN_PANEL_LIST (self->panel_list),
                              posman_panel_list_main);
}

static void
panel_list_row_activated_cb(PosmanPanelList   *list,
                            GtkListBox        *box,
                            GtkListBoxRow     *row,
                            PosmanWindow      *self)
{
  RowCustData  *data = g_object_get_data(G_OBJECT(row),"data");
  GPtrArray    *rows = get_cmnds (self,data->id);
  GListStore   *lstr = g_list_store_new(GTK_TYPE_LIST_BOX_ROW);

  g_list_store_splice(lstr,
                      0,0,rows->pdata,
                      rows->len);

  posman_panel_list_set_model_cmnd(POSMAN_PANEL_LIST (list),G_OBJECT (lstr));
  g_object_unref (lstr);

  posman_panel_list_set_view (list, posman_panel_list_cust);
}
/* object vfonc */

static void posman_window_finalize(GObject *object)
{
  PosmanWindow *self = POSMAN_WINDOW (object);
  sqlite3_close(self->db);

  G_OBJECT_CLASS (posman_window_parent_class)->finalize (object);
}

static void
posman_window_update_cust(PosmanWindow *self)
{
  GPtrArray    *rows = get_custs (self);
  GListStore   *lstr = g_list_store_new(GTK_TYPE_LIST_BOX_ROW);

  g_list_store_splice(lstr,
                      0,0,rows->pdata,
                      rows->len);

  posman_panel_list_set_model_cust(POSMAN_PANEL_LIST (self->panel_list),G_OBJECT (lstr));
  g_object_unref (lstr);

}

static void posman_window_constructed(GObject *object)
{
  PosmanWindow *self = POSMAN_WINDOW (object);

  posman_window_update_cust(self);

  G_OBJECT_CLASS (posman_window_parent_class)->constructed (object);
}

static void
posman_window_class_init (PosmanWindowClass *klass)
{
  GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS (klass);
  GObjectClass    *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize    = posman_window_finalize;
  object_class->constructed = posman_window_constructed;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/pos/manager/posman-window.ui");

  gtk_widget_class_bind_template_child (widget_class, PosmanWindow, previous_button);
  gtk_widget_class_bind_template_child (widget_class, PosmanWindow, panel_list);
  gtk_widget_class_bind_template_child (widget_class, PosmanWindow, action_menu);
  gtk_widget_class_bind_template_child (widget_class, PosmanWindow, select_button);

  gtk_widget_class_bind_template_callback (widget_class, panel_list_view_changed_cb);
  gtk_widget_class_bind_template_callback (widget_class, previous_button_clicked_cb);
  gtk_widget_class_bind_template_callback (widget_class, panel_list_row_activated_cb);
  gtk_widget_class_bind_template_callback (widget_class, add_pressed_cb);
  gtk_widget_class_bind_template_callback (widget_class, select_pressed_cb);

  g_type_ensure(POSMAN_TYPE_PANEL_LIST);
  g_type_ensure(POSMAN_TYPE_ACTION_MENU);
}

static void
posman_window_init (PosmanWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
  posman_window_init_database(self);
}


