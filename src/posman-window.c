#include "posman-config.h"
#include "posman-window.h"
#include "posman-panel-list.h"
#include "posman-action-menu-cust.h"
#include "posman-action-menu-cmnd.h"
#include <posman-panel-row-cust.h>
#include <sqlite3.h>

struct _PosmanWindow
{
  GtkApplicationWindow  parent_instance;

  /* Template widgets */
  GtkWidget             *previous_button;
  GtkWidget             *panel_list;

  GtkWidget             *action_menu_cust;
  GtkWidget             *select_button;

  GtkWidget             *content_label;
  GtkWidget             *info_bar;

  GObject               *cust_model;
  GListStore            *cmnd_model;


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

/* callback */
static void
previous_button_clicked_cb()
{

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

/* object vfonc */

static void posman_window_finalize(GObject *object)
{
  PosmanWindow *self = POSMAN_WINDOW (object);
  sqlite3_close(self->db);

  G_OBJECT_CLASS (posman_window_parent_class)->finalize (object);
}
static void
remove_cust(GSimpleAction *simple,
            GVariant      *parameter,
            gpointer       user_data)
{
  PosmanWindow    *self = POSMAN_WINDOW (user_data);

  posman_window_remove_cust_with_id (self,
                                     g_variant_get_int64(parameter));
}


static GActionGroup *
create_action_group (PosmanWindow *self)
{
  GSimpleActionGroup *group;
  const GActionEntry entries[] = {
    { "removecust",remove_cust,"x"    },
  };

  group = g_simple_action_group_new ();
  g_action_map_add_action_entries (G_ACTION_MAP (group),
                                   entries,
                                   G_N_ELEMENTS (entries),
                                   self);

  return G_ACTION_GROUP (group);
}

static GPtrArray *
create_cust_ptrs(PosmanWindow *self)
{
  g_return_val_if_fail (POSMAN_IS_WINDOW (self),NULL);

  GPtrArray     *custs_ptr;
  sqlite3_stmt  *res;
  int           rc;

  custs_ptr = g_ptr_array_new ();

  rc = sqlite3_prepare_v2(self->db, "select id,full_name from customer;", -1, &res, 0);
  if(rc != SQLITE_OK)
    {
      fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
      return NULL;
    }

  while(sqlite3_step(res) != SQLITE_DONE)
    {
      GtkWidget   *panel;
      panel = g_object_new (POSMAN_TYPE_PANEL_ROW_CUST,
                            "cust-name",
                            sqlite3_column_text (res, COL_CUST_NAME),
                            "cust-id",
                            sqlite3_column_int64 (res, COL_CUST_ID),
                            NULL);
      g_ptr_array_add(custs_ptr,panel);
    }

  sqlite3_finalize(res);
  return custs_ptr;
}

static void
fill_model_with_custs(GListStore  *list,GPtrArray  *array)
{
  g_return_if_fail (G_IS_LIST_STORE (list));

  g_list_store_splice(list,
                      0,
                      0,
                      array->pdata,
                      array->len);
  g_ptr_array_unref(array);

}

static void posman_window_constructed(GObject *object)
{
  PosmanWindow *self  = POSMAN_WINDOW (object);
  GActionGroup *group = create_action_group(self);

  G_OBJECT_CLASS (posman_window_parent_class)->constructed (object);

  self->cust_model  =(GObject*) g_list_store_new (POSMAN_TYPE_PANEL_ROW_CUST);
  fill_model_with_custs (G_LIST_STORE (self->cust_model),
                         create_cust_ptrs (self));
  posman_panel_list_set_list_stor_cust (POSMAN_PANEL_LIST (self->panel_list),
                                        self->cust_model);

  gtk_widget_insert_action_group(GTK_WIDGET (self),
                                 "win",
                                 group);
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
  gtk_widget_class_bind_template_child (widget_class, PosmanWindow, action_menu_cust);
  gtk_widget_class_bind_template_child (widget_class, PosmanWindow, select_button);
  gtk_widget_class_bind_template_child (widget_class, PosmanWindow, content_label);
  gtk_widget_class_bind_template_child (widget_class, PosmanWindow, info_bar);

  gtk_widget_class_bind_template_callback (widget_class, panel_list_view_changed_cb);
  gtk_widget_class_bind_template_callback (widget_class, previous_button_clicked_cb);
  /*gtk_widget_class_bind_template_callback (widget_class, panel_list_row_activated_cb);
  gtk_widget_class_bind_template_callback (widget_class, add_pressed_cb);
  gtk_widget_class_bind_template_callback (widget_class, select_pressed_cb);
  gtk_widget_class_bind_template_callback (widget_class, remove_pressed_cb);
  gtk_widget_class_bind_template_callback (widget_class, remove_custs_pressed_cb);*/

  g_type_ensure(POSMAN_TYPE_PANEL_LIST);
  g_type_ensure(POSMAN_TYPE_ACTION_MENU_CUST);
  g_type_ensure(POSMAN_TYPE_ACTION_MENU_CMND);
}

static void
posman_window_init (PosmanWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
  posman_window_init_database(self);
}
void
posman_window_remove_cust_with_id(PosmanWindow *self, gint64  id)
{
  guint     n = g_list_model_get_n_items(G_LIST_MODEL (self->cust_model));
  gboolean  not_removed = TRUE;
  guint     count = 0;
  while (not_removed || count >= n)
    {
      PosmanPanelRowCust  *row =(PosmanPanelRowCust*)
      g_list_model_get_item(G_LIST_MODEL (self->cust_model),
                            count);
      if(posman_panel_row_cust_get_id(row) == id)
        {
          int   rc;
          g_autofree gchar *sql;
          char *err_msg = 0;

          posman_panel_list_remove_row_cust(POSMAN_PANEL_LIST (self->panel_list),row);

          sql = g_strdup_printf("DELETE FROM customer WHERE id = %lu;",id);
          rc = sqlite3_exec(self->db, sql, 0, 0, &err_msg);
          if (rc != SQLITE_OK )
            {
            fprintf(stderr, "SQL error: %s\n", err_msg);
            sqlite3_free(err_msg);

            return;
            }

          return;
        }
      count++;
    }
}
