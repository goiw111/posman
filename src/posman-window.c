#include "posman-config.h"
#include "posman-window.h"
#include "posman-panel-list.h"
#include "posman-action-menu-cust.h"
#include "posman-action-menu-cmnd.h"
#include <posman-panel-row-cust.h>
#include <posman-action-area.h>
#include <sqlite3.h>

struct _PosmanWindow {
  GtkApplicationWindow parent_instance;

  /* Template widgets */
  GtkWidget             *previous_button;
  GtkWidget             *panel_list;
  GtkWidget             *action_area_container;

  GtkWidget             *action_menu_cust;
  GtkWidget             *action_menu_cmnd;
  GtkWidget             *select_button;

  GtkWidget             *content_label;
  GtkWidget             *info_bar;

  GObject               *cust_model;
  GListStore            *cmnd_model;
  GtkWidget             *domain_model;
  GtkWidget             *action_area_items;

  /* database */
  sqlite3               *db;
};

enum {
  COL_CUST_ID,
  COL_CUST_NAME,

  N_CUST_COL
};

enum {
  COL_CMND_ID,
  COL_CMND_STATE,
  COL_CMND_DATE,
  COL_CMND_TOTAL,

  N_CMND_COL
};

GdkPixbuf *item,*items;

G_DEFINE_TYPE(PosmanWindow, posman_window, GTK_TYPE_APPLICATION_WINDOW)

static void
posman_window_init_database(PosmanWindow *self)
{
  g_autofree char *dir = NULL;

  dir = g_strdup_printf("%s/" "posman.db", g_get_user_data_dir());

  int rc = sqlite3_open(dir, &(self->db));

  if (rc != SQLITE_OK)
    {
      g_warning("Cannot open database: %s\n", sqlite3_errmsg(self->db));
      sqlite3_close(self->db);
    }
}

static void
fill_action_area(PosmanWindow *self,
                 GtkWidget    *iconview,
                 gint64        id)
{
  gint                rc;
  sqlite3_stmt        *res;
  g_autofree gchar*   buffer;
  g_autofree gchar*   str_id = g_new(gchar,G_ASCII_DTOSTR_BUF_SIZE);
  str_id = (id == -1)?NULL:g_ascii_dtostr(str_id,sizeof (str_id),(gdouble)id);

  buffer = g_strdup_printf ("select id,name,false as type from category "\
                            "where cat_id is %s union "\
                            "select p.id,p.full_name, true as type from stock as s "\
                            "inner join product as p on s.prod_id = p.id and p.category_id is %s "\
                            "left JOIN order_items as o on s.id = o.stock_id group by s.id "\
                            "having sum(o.quantity) < sum(s.quantity) or o.id is null order by type,id",
                            str_id,str_id);

  rc = sqlite3_prepare_v2(self->db,buffer,-1, &res, 0);
  if (rc != SQLITE_OK)
    {
      fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
      sqlite3_finalize(res);
      return;
    }

  gtk_list_store_clear(GTK_LIST_STORE(iconview));
  while (sqlite3_step(res) != SQLITE_DONE)
    {
      gtk_list_store_insert_with_values(GTK_LIST_STORE(iconview),
                                        NULL,
                                        -1,
                                        0, sqlite3_column_int64(res, 0),
                                        1, sqlite3_column_text(res, 1),
                                        2, sqlite3_column_int(res,2)? item: items,
                                        3, (gboolean)sqlite3_column_int(res,2),
                                        -1);
    }
  sqlite3_finalize(res);
}

static void
fill_combobox_model(PosmanWindow      *self,
                    PosmanActionArea  *action,
                    GtkComboBox       *combobox,
                    gint64            id)
{
  GtkTreeModel        *model;
  g_autofree gchar    *buffer;
  gint                rc;
  sqlite3_stmt        *res;
  gint                dmn_id;

  model = gtk_combo_box_get_model (combobox);
  dmn_id = posman_action_area_get_cust_dmn (action);

  buffer = g_strdup_printf ("");

  rc = sqlite3_prepare_v2(self->db,buffer,-1, &res, 0);
  if (rc != SQLITE_OK)
    {
      fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
      sqlite3_finalize(res);
      return;
    }
}
/* callback */

void
item_activated_callback(PosmanActionArea  *action_area,
                        GtkIconView       *icon_view,
                        GtkTreePath       *path,
                        gpointer          user_data)
{
  PosmanWindow        *self = POSMAN_WINDOW(user_data);
  GtkTreeModel        *model;
  GtkComboBox         *combo;
  GtkTreeIter         iter;
  gboolean            is_item;
  gint64              id;


  model = gtk_icon_view_get_model (icon_view);
  combo = posman_action_area_get_combo (action_area);
  gtk_tree_model_get_iter(model,&iter,path);
  gtk_tree_model_get(model,&iter,3,&is_item,0,&id,-1);

  if(is_item)
    fill_combobox_model(self,action_area,combo,id);
  else
    fill_action_area(self,(GtkWidget*)model,id);

  /*GtkTreeModel        *model;
  GtkTreeIter         iter;
  gint64              id;
  g_autofree gchar    *buffer;
  gint                rc;
  sqlite3_stmt        *res;

  model = gtk_icon_view_get_model (icon_view);
  gtk_tree_model_get_iter(model,&iter,path);
  gtk_tree_model_get(model,&iter,0,&id,-1);
  buffer = g_strdup_printf ("");

  rc = sqlite3_prepare_v2(self->db,buffer,-1, &res, 0);
  if (rc != SQLITE_OK)
    {
      fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
      sqlite3_finalize(res);
      return;
    }*/

}

static void
panel_list_view_changed_cb(PosmanPanelList *panel_list,
                           GParamSpec  *pspec,
                           PosmanWindow    *self)
{
  gboolean is_main_view;
  gboolean is_cust_view;
  gboolean is_addc_view;

  is_main_view = posman_panel_list_get_view(panel_list) == posman_panel_list_main;
  is_cust_view = posman_panel_list_get_view(panel_list) == posman_panel_list_cust;
  is_addc_view = posman_panel_list_get_view(panel_list) == posman_panel_list_add_cust;

  gtk_widget_set_visible(self->previous_button, !is_main_view);
  gtk_widget_set_visible(self->select_button, is_addc_view);
  gtk_widget_set_visible(self->action_menu_cust, is_main_view);
  gtk_widget_set_visible(self->action_menu_cmnd, is_cust_view);
}

/* object vfonc */

static void posman_window_finalize(GObject *object)
{
  PosmanWindow *self = POSMAN_WINDOW(object);

  sqlite3_close(self->db);

  G_OBJECT_CLASS(posman_window_parent_class)->finalize(object);
}
static void
remove_cust(GSimpleAction *simple,
            GVariant      *parameter,
            gpointer user_data)
{
  PosmanWindow    *self = POSMAN_WINDOW(user_data);

  posman_window_remove_cust_with_id(self,
                                    g_variant_get_int64(parameter));
}

static void
show_add_cust(GSimpleAction *simple,
              GVariant      *parameter,
              gpointer user_data)
{
  PosmanWindow    *self = POSMAN_WINDOW(user_data);

  posman_panel_list_set_view(POSMAN_PANEL_LIST(self->panel_list), posman_panel_list_add_cust);
}

static void
previous_button_pressed(GSimpleAction *simple,
                        GVariant      *parameter,
                        gpointer user_data)
{
  PosmanWindow    *self = POSMAN_WINDOW(user_data);

  if (posman_panel_list_is_add_cust_in_prog(POSMAN_PANEL_LIST(self->panel_list)) &&
      posman_panel_list_get_view(POSMAN_PANEL_LIST(self->panel_list)) == posman_panel_list_add_cust)
    {
      GtkWidget       *dialog;
      int result;
      GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
      dialog = gtk_message_dialog_new(GTK_WINDOW(self),
                                      flags,
                                      GTK_MESSAGE_WARNING,
                                      GTK_BUTTONS_YES_NO,
                                      "are you sure that you want to cancel");
      result = gtk_dialog_run(GTK_DIALOG(dialog));
      gtk_widget_destroy(dialog);
      if (result == GTK_RESPONSE_NO)
        return;
      posman_panel_list_clear_add_cust(POSMAN_PANEL_LIST(self->panel_list));
    }

  posman_panel_list_set_view(POSMAN_PANEL_LIST(self->panel_list), posman_panel_list_main);
}
static void
save_button_pressed(GSimpleAction *simple,
                    GVariant      *parameter,
                    gpointer user_data)
{
  PosmanWindow               *self = POSMAN_WINDOW(user_data);
  gboolean warning;
  g_autofree const gchar     *query;

  query = posman_panel_list_get_sql_query(POSMAN_PANEL_LIST(self->panel_list),
                                          &warning);
  if (warning)
    {
      char *err_msg = 0;
      int rc;
      GtkWidget   *row;

      rc = sqlite3_exec(self->db, query, 0, 0, &err_msg);
      if (rc != SQLITE_OK)
        {
          fprintf(stderr, "SQL error: %s\n", err_msg);
          return;
        }
      row = g_object_new(POSMAN_TYPE_PANEL_ROW_CUST,
                         "cust-id", sqlite3_last_insert_rowid(self->db),
                         "cust-name", posman_panel_list_add_cust_get_name(POSMAN_PANEL_LIST(self->panel_list)),
                         NULL);
      g_list_store_append(G_LIST_STORE(self->cust_model),
                          row);
    }
  else
    {
      gtk_label_set_text(GTK_LABEL(self->content_label), query);
      gtk_widget_set_visible(self->info_bar, TRUE);
      return;
    }

  posman_panel_list_clear_add_cust(POSMAN_PANEL_LIST(self->panel_list));
  posman_panel_list_set_view(POSMAN_PANEL_LIST(self->panel_list), posman_panel_list_main);
}

static void
list_box_row_activated(GSimpleAction *simple,
                       GVariant      *parameter,
                       gpointer user_data)
{
  PosmanWindow       *self = POSMAN_WINDOW(user_data);
  gint64             cust_id = g_variant_get_int64(parameter);
  g_autofree gchar   *cust_id_str = g_strdup_printf("%li", cust_id);
  GtkWidget          *child = gtk_stack_get_child_by_name(GTK_STACK (self->action_area_container),
                                 cust_id_str);
  PosmanPanelRowCust *row = posman_panel_list_get_panel_row_cust_by_id(POSMAN_PANEL_LIST (self->panel_list),
                                                          cust_id);
  if(child != NULL)
    {
      gtk_stack_set_visible_child(GTK_STACK (self->action_area_container),
                                  child);
      posman_panel_list_set_view(POSMAN_PANEL_LIST(self->panel_list),
                                 posman_panel_list_cust);
      return;
    }

  PosmanActionArea   *cust_area =
  g_object_new(POSMAN_TYPE_ACTION_AREA,
               "cust-id",cust_id,
               NULL);

  posman_panel_row_cust_set_action_area (row, cust_area);
  posman_panel_list_set_cust_id(POSMAN_PANEL_LIST(self->panel_list),
                                cust_id);

  gtk_stack_add_named(GTK_STACK(self->action_area_container),
                      GTK_WIDGET(cust_area),
                      cust_id_str);

  gtk_stack_set_visible_child(GTK_STACK(self->action_area_container),
                              GTK_WIDGET(cust_area));

  GList * list =
  gtk_container_get_children(GTK_CONTAINER (self->action_area_container));
  list = g_list_first(list);
  while (list != NULL)
    {

      if(posman_panel_list_get_cust_id (POSMAN_PANEL_LIST(self->panel_list)) !=
         posman_action_area_get_cust_id (POSMAN_ACTION_AREA (list->data)))
        {
          gtk_container_remove(GTK_CONTAINER (self->action_area_container),
                             GTK_WIDGET (list->data));
          g_print("here\n");
        }

      list = g_list_next (list);
    }
  g_list_free(list);

  posman_panel_list_set_view(POSMAN_PANEL_LIST(self->panel_list),
                             posman_panel_list_cust);
}

static void
add_cmnd_button_pressed(GSimpleAction *simple,
                        GVariant      *parameter,
                        gpointer user_data)
{
  PosmanWindow            *self = POSMAN_WINDOW(user_data);
  gint64                  cust_id;
  PosmanPanelRowCust      *row_cust;
  PosmanActionArea        *action_area;
  GtkWidget               *iconview;

  cust_id = posman_panel_list_get_cust_id(POSMAN_PANEL_LIST (self->panel_list));
  row_cust = posman_panel_list_get_panel_row_cust_by_id (POSMAN_PANEL_LIST (self->panel_list),
                                                         cust_id);
  action_area = posman_panel_row_cust_get_action_area(row_cust);
  iconview  = posman_action_area_get_items_viewer(action_area);

  posman_action_area_set_view(POSMAN_ACTION_AREA (action_area),
                              posman_action_area_view_add_cmnd);
  g_signal_connect(action_area,"item-activated",G_CALLBACK(item_activated_callback),self);
  fill_action_area(self,iconview,-1);
}


static GActionGroup *
create_action_group(PosmanWindow *self)
{
  GSimpleActionGroup *group;
  const GActionEntry entries[] = {
    { "removecust",      remove_cust,             "x"},
    { "showaddcust",     show_add_cust               },
    { "previouspressed", previous_button_pressed     },
    { "savepressed",     save_button_pressed         },
    { "addcmnd",         add_cmnd_button_pressed     },
    { "rowactivated",    list_box_row_activated,  "x"}
  };

  group = g_simple_action_group_new();
  g_action_map_add_action_entries(G_ACTION_MAP(group),
                                  entries,
                                  G_N_ELEMENTS(entries),
                                  self);

  return G_ACTION_GROUP(group);
}

static GPtrArray *
create_cust_ptrs(PosmanWindow *self)
{
  g_return_val_if_fail(POSMAN_IS_WINDOW(self), NULL);

  GPtrArray     *custs_ptr;
  sqlite3_stmt  *res;
  int rc;

  custs_ptr = g_ptr_array_new();

  rc = sqlite3_prepare_v2(self->db, "select id,full_name from customer;", -1, &res, 0);
  if (rc != SQLITE_OK)
    {
      fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
      sqlite3_finalize(res);
      return NULL;
    }

  while (sqlite3_step(res) != SQLITE_DONE)
    {
      GtkWidget   *panel;
      panel = g_object_new(POSMAN_TYPE_PANEL_ROW_CUST,
                           "cust-name",
                           sqlite3_column_text(res, COL_CUST_NAME),
                           "cust-id",
                           sqlite3_column_int64(res, COL_CUST_ID),
                           NULL);
      g_ptr_array_add(custs_ptr, panel);
    }

  sqlite3_finalize(res);
  return custs_ptr;
}

static void
fill_model_with_custs(GListStore  *list, GPtrArray  *array)
{
  g_return_if_fail(G_IS_LIST_STORE(list));

  g_list_store_splice(list,
                      0,
                      0,
                      array->pdata,
                      array->len);
  g_ptr_array_unref(array);
}

static void posman_window_constructed(GObject *object)
{
  PosmanWindow *self = POSMAN_WINDOW(object);
  GActionGroup *group = create_action_group(self);

  G_OBJECT_CLASS(posman_window_parent_class)->constructed(object);

  self->cust_model = (GObject*)g_list_store_new(POSMAN_TYPE_PANEL_ROW_CUST);
  fill_model_with_custs(G_LIST_STORE(self->cust_model),
                        create_cust_ptrs(self));
  posman_panel_list_set_list_stor_cust(POSMAN_PANEL_LIST(self->panel_list),
                                       self->cust_model);

  self->domain_model = (GtkWidget*)gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
  fill_model_with_domains(POSMAN_WINDOW(self));
  posman_panel_list_set_model_domain(POSMAN_PANEL_LIST(self->panel_list),
                                     GTK_LIST_STORE(self->domain_model));
  gtk_widget_insert_action_group(GTK_WIDGET(self),
                                 "win",
                                 group);
}

static void
posman_window_class_init(PosmanWindowClass *klass)
{
  GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(klass);
  GObjectClass    *object_class = G_OBJECT_CLASS(klass);

  object_class->finalize = posman_window_finalize;
  object_class->constructed = posman_window_constructed;

  gtk_widget_class_set_template_from_resource(widget_class, "/org/pos/manager/posman-window.ui");

  gtk_widget_class_bind_template_child(widget_class, PosmanWindow, previous_button);
  gtk_widget_class_bind_template_child(widget_class, PosmanWindow, panel_list);
  gtk_widget_class_bind_template_child(widget_class, PosmanWindow, action_menu_cust);
  gtk_widget_class_bind_template_child(widget_class, PosmanWindow, action_menu_cmnd);
  gtk_widget_class_bind_template_child(widget_class, PosmanWindow, select_button);
  gtk_widget_class_bind_template_child(widget_class, PosmanWindow, content_label);
  gtk_widget_class_bind_template_child(widget_class, PosmanWindow, info_bar);
  gtk_widget_class_bind_template_child(widget_class, PosmanWindow, action_area_container);

  gtk_widget_class_bind_template_callback(widget_class, panel_list_view_changed_cb);
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
posman_window_init(PosmanWindow *self)
{
  gtk_widget_init_template(GTK_WIDGET(self));
  posman_window_init_database(self);
  item  = gdk_pixbuf_new_from_resource_at_scale("/org/pos/manager/data/box.png",48,48,FALSE,NULL);
  items = gdk_pixbuf_new_from_resource_at_scale("/org/pos/manager/data/boxes.png",48,48,FALSE,NULL);
}
void
posman_window_remove_cust_with_id(PosmanWindow *self, gint64 id)
{
  guint n = g_list_model_get_n_items(G_LIST_MODEL(self->cust_model));
  gboolean not_removed = TRUE;
  guint count = 0;

  while (not_removed || count >= n)
    {
      PosmanPanelRowCust  *row = (PosmanPanelRowCust*)
                                 g_list_model_get_item(G_LIST_MODEL(self->cust_model),
                                                       count);
      if (posman_panel_row_cust_get_id(row) == id)
        {
          int rc;
          g_autofree gchar *sql;
          char *err_msg = 0;

          posman_panel_list_remove_row_cust(POSMAN_PANEL_LIST(self->panel_list), row);

          sql = g_strdup_printf("DELETE FROM customer WHERE id = %lu;", id);
          rc = sqlite3_exec(self->db, sql, 0, 0, &err_msg);
          if (rc != SQLITE_OK)
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

void
fill_model_with_domains(PosmanWindow  *self)
{
  int rc;
  sqlite3_stmt  *res;

  rc = sqlite3_prepare_v2(self->db, "select id,name from domains;", -1, &res, 0);
  if (rc != SQLITE_OK)
    {
      fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(self->db));
      sqlite3_finalize(res);
      return;
    }
  while (sqlite3_step(res) != SQLITE_DONE)
    {
      gtk_list_store_insert_with_values(GTK_LIST_STORE(self->domain_model),
                                        NULL,
                                        -1,
                                        0, sqlite3_column_text(res, 0),
                                        1, sqlite3_column_text(res, 1),
                                        -1);
    }
}

