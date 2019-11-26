
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
  GObject               *list_stor_cust;
  GObject               *list_stor_cmnd;

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

static int
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

static gpointer *
get_custs(PosmanWindow *self)
{
  sqlite3_stmt *stmt;
  int          rc;
  int          i;

  rc = sqlite3_prepare_v2(self->db,"SELECT id, full_name FROM customer;",-1,
                          &stmt,NULL);
  if (rc != SQLITE_OK)
    {
      g_warning ("Failed to execute statement: %s\n", sqlite3_errmsg(self->db));
      return NULL;
    }
  i = sqlite3_data_count(stmt);
  gpointer rows[i];

  int count = 0;
  while(sqlite3_step(stmt) != SQLITE_DONE)
    {
    rows[count] = row_cust_data_new (sqlite3_column_text (stmt, COL_CUST_ID),
                                     sqlite3_column_text (stmt, COL_CUST_NAME));
    count++;
    }

  sqlite3_finalize(stmt);
  return rows[i];
}

static gpointer *
get_cmnds(PosmanWindow *self,
          gchar        *cust_id)
{
  sqlite3_stmt *stmt;
  int          rc;
  int          i;
  g_autofree gchar    *sql = NULL;


  sql = g_strdup_printf("select id,status,order_date from orders where cust_id = %s;",cust_id);
  rc = sqlite3_prepare_v2(self->db,sql,-1,
                          &stmt,NULL);
  if (rc != SQLITE_OK)
    {
      g_warning ("Failed to execute statement: %s\n", sqlite3_errmsg(self->db));
      return NULL;
    }
  i = sqlite3_data_count(stmt);
  gpointer rows[i];

  int count = 0;
  while(sqlite3_step(stmt) != SQLITE_DONE)
    {
    rows[count] = row_cmnd_data_new (sqlite3_column_text (stmt, COL_CMND_ID),
                                     sqlite3_column_text (stmt, COL_CMND_STATE),
                                     sqlite3_column_text (stmt, COL_CMND_DATE),
                                     (const char*)"NULL");
    count++;
    }

  sqlite3_finalize(stmt);
  return rows[i];
}
/* callback */

void panel_list_view_changed_cb (PosmanPanelList *panel_list,
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

/* object vfonc */

static void posman_window_finalize(GObject *object)
{
  PosmanWindow *self = POSMAN_WINDOW (object);
  sqlite3_close(self->db);

  G_OBJECT_CLASS (posman_window_parent_class)->finalize (object);
}

static void posman_window_constructed(GObject *object)
{
  PosmanWindow *self = POSMAN_WINDOW (object);
  gpointer     *rows= get_custs (self);

  g_list_store_splice(G_LIST_STORE (self->list_stor_cust),
                      0,0,rows,
                      G_N_ELEMENTS(rows));

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
  gtk_widget_class_bind_template_child (widget_class, PosmanWindow, list_stor_cust);
  gtk_widget_class_bind_template_child (widget_class, PosmanWindow, list_stor_cmnd);
  gtk_widget_class_bind_template_callback (widget_class, panel_list_view_changed_cb);
  gtk_widget_class_bind_template_callback (widget_class, previous_button_clicked_cb);

  g_type_ensure(POSMAN_TYPE_PANEL_LIST);
  g_type_ensure(POSMAN_TYPE_ACTION_MENU);
}

static void
posman_window_init (PosmanWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
  posman_window_init_database(self);
}


