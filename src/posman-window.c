#include "posman-config.h"
#include "posman-window.h"
#include "posman-panel-list.h"
#include "posman-panel-list-row.h"
#include "posman-action-controler.h"
#include "posman-item.h"
#include "posman-item-list-row.h"
#include <sqlite3.h>

struct _PosmanWindow {
  GtkApplicationWindow parent_instance;

  /* Template widgets */
  GtkWidget             *previous_button;
  GtkWidget             *panel_list;
  GtkWidget             *info_bar;
  GtkWidget             *content_label;
  GtkWidget             *select_button;
  GtkWidget             *action_area_container;
  GtkWidget             *action_listbox;

  /* database */
  sqlite3               *db;
};

G_DEFINE_TYPE(PosmanWindow, posman_window, GTK_TYPE_APPLICATION_WINDOW)

static void
posman_window_init_database(PosmanWindow *self);
static void
posman_window_Notify(PosmanWindow *self,const gchar *info);
static PosmanAddCmnd *
posman_window_cmnd_add_is_active(PosmanWindow *self,gint  cmnd_id);

/*
 * Auxiliary methods
 */

static void
posman_window_init_cust(PosmanWindow     *self,
                        PosmanPanelList  *panel)
{
  gint                 rc,rc_chaque,rc_cash;
  sqlite3_stmt        *res;
  g_autofree gchar    *buffer;
  g_autofree gchar    *buffer_cheque;
  g_autofree gchar    *buffer_cash;
  sqlite3_stmt        *res_rest_cheque;
  sqlite3_stmt        *res_rest_cash;

  buffer = g_strdup_printf ("select c.id,c.full_name,ifnull(sum(quantity*price),0),c.domain_id from customer as c "\
                            "left join orders as o on o.cust_id = c.id "\
                            "left join order_items as oi on oi.order_id = o.id "\
                            "group by c.id ");
  buffer_cheque = g_strdup_printf ("select ifnull(sum(pc.value),0) from customer as c "\
                                   "left join payment as p on p.cust_id = c.id "\
                                   "left join payment_cash as pc on p.id = pc.pay_id "\
                                   "group by c.id ");
  buffer_cash = g_strdup_printf ("select ifnull(sum(pch.the_value),0) from customer as c "\
                                 "left join payment as p on p.cust_id = c.id "\
                                 "left join payment_cheque as pch on p.id = pch.payment_id "\
                                 "group by c.id ");

  rc = sqlite3_prepare_v2(self->db,buffer,-1, &res, 0);
  if (rc != SQLITE_OK)
    {
      fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
      sqlite3_finalize(res);
      return;
    }
  rc_cash = sqlite3_prepare_v2(self->db,buffer_cash,-1, &res_rest_cash, 0);
  if (rc_cash != SQLITE_OK)
    {
      fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
      sqlite3_finalize(res_rest_cash);
      return;
    }
  rc_chaque = sqlite3_prepare_v2(self->db,buffer_cheque,-1, &res_rest_cheque, 0);
  if (rc_chaque != SQLITE_OK)
    {
      fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
      sqlite3_finalize(res_rest_cheque);
      return;
    }

  while (sqlite3_step(res) != SQLITE_DONE
         && sqlite3_step(res_rest_cash) != SQLITE_DONE
         && sqlite3_step(res_rest_cheque) != SQLITE_DONE)
    {
      gdouble        rest;
      GtkWidget     *cust_row;

      rest = sqlite3_column_int(res, 2)-
      (sqlite3_column_int(res_rest_cheque, 0)
       +sqlite3_column_int(res_rest_cash, 0));

      cust_row = row_cust_data_new(sqlite3_column_int(res, 0),
                                   sqlite3_column_int(res, 3),
                                   sqlite3_column_text (res, 1),rest)->row;
      posman_panel_list_insert_cust(panel,cust_row);
    }
  sqlite3_finalize(res_rest_cash);
  sqlite3_finalize(res_rest_cheque);
  sqlite3_finalize(res);
}

/* callback */

void
domain_model_init(PosmanWindow    *self,
                  GtkListStore    *model)
{
  gint           rc;
  sqlite3_stmt  *res;

  gtk_list_store_clear(model);
  rc = sqlite3_prepare_v2(self->db,"select id,name from domains",-1, &res, 0);
  if (rc != SQLITE_OK)
    {
      fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
      sqlite3_finalize(res);
      return;
    }
    while (sqlite3_step(res) != SQLITE_DONE)
    {
      gtk_list_store_insert_with_values(model,
                                        NULL,-1,
                                        1,sqlite3_column_text(res, 0),
                                        0,sqlite3_column_text(res, 1),-1);
    }
  sqlite3_finalize(res);
}

void
view_changed_cb(GObject    *gobject,
                GParamSpec *pspec,
                gpointer    user_data)
{
  PosmanWindow      *self = POSMAN_WINDOW(user_data);
  PosmanPanelList   *list = POSMAN_PANEL_LIST (gobject);
  if(posman_panel_list_get_view(list) != posman_panel_list_main)
    gtk_widget_set_visible (self->previous_button, TRUE );
  else
    gtk_widget_set_visible (self->previous_button, FALSE );
  if(posman_panel_list_get_view(list) == posman_panel_list_add_cust)
    {
      GtkListStore  *model;
      gtk_widget_set_visible (self->select_button, TRUE );
      model = posman_panel_list_get_domn_model (list);
      domain_model_init(self,model);
    }
  else
    gtk_widget_set_visible (self->select_button, FALSE );

}

void
previous_button_cb(GtkButton    *button,
                   gpointer      user_data)
{
  PosmanWindow *self = POSMAN_WINDOW(user_data);

  posman_panel_list_set_view(POSMAN_PANEL_LIST (self->panel_list),
                             posman_panel_list_main);
}

void
select_button_cb(GtkButton    *button,
                 gpointer      user_data)
{
  PosmanWindow *self = POSMAN_WINDOW(user_data);
  if(!posman_panel_list_check_data(POSMAN_PANEL_LIST (self->panel_list),
                                   self->content_label))
    gtk_widget_set_visible (self->info_bar, TRUE);
}

void
add_cust_cb(PosmanPanelList   *list,
            gchar             *name,
            gchar             *address,
            gchar             *phone,
            gchar             *domn_id,
            gchar             *description,
            gpointer           user_data)
{
  PosmanWindow  *self = POSMAN_WINDOW(user_data);
  gchar         *buffer;
  gint           rc;
  sqlite3_stmt  *res;


  buffer = g_strdup_printf ("select full_name from customer where full_name = '%s'",name);
  rc = sqlite3_prepare_v2(self->db,buffer,-1, &res, 0);
  g_free(buffer);
  if (rc != SQLITE_OK)
    {
      fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
      sqlite3_finalize(res);
      return;
    }
  sqlite3_step(res);
  if(sqlite3_column_text (res, 0) != NULL)
    {
      gtk_label_set_text (GTK_LABEL (self->content_label),
                          "sorry that name is already exist");
      gtk_widget_set_visible (self->info_bar, TRUE);
      sqlite3_finalize(res);
      return;
    }
  sqlite3_finalize(res);
  buffer = g_strdup_printf ("INSERT INTO customer "\
                            "(full_name,address,domain_id,description,phone) "\
                            "VALUES ('%s','%s',%s,'%s','%s')",
                            name,address,domn_id,description,phone);
  rc = sqlite3_prepare_v2(self->db,buffer,-1, &res, 0);
  g_free(buffer);
  if (rc != SQLITE_OK)
    {
      fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
      sqlite3_finalize(res);
      return;
    }
  sqlite3_step(res);
  GtkWidget     *cust_row;

  cust_row = row_cust_data_new(sqlite3_last_insert_rowid(self->db),
                               g_strtod(domn_id,NULL),
                               name,0.0)->row;
  posman_panel_list_insert_cust(list,
                                cust_row);
  sqlite3_finalize(res);
  posman_panel_list_set_view (list, posman_panel_list_main);
}

void
remove_cust_cb(PosmanPanelList    *list,
               GtkListBoxRow      *row,
               GtkListBox         *list_cust,
               gpointer            user_data)
{
  PosmanWindow *self = POSMAN_WINDOW(user_data);
  gchar        *buffer;
  gint          rc;
  sqlite3_stmt *res;
  gint          cust_id;
  CustRowData  *row_data;
  char         *err_msg = 0;

  row_data = g_object_get_data (G_OBJECT (row),"data");
  cust_id  = row_data->cust_id;

  buffer = g_strdup_printf ("select id from orders where cust_id is %d",cust_id);
  rc = sqlite3_prepare_v2(self->db,buffer,-1, &res, 0);
  g_free(buffer);
  if (rc != SQLITE_OK)
    {
      fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
      sqlite3_finalize(res);
      return;
    }
  sqlite3_step(res);
  if(sqlite3_column_text (res, 0) != NULL)
    {
      gtk_label_set_text (GTK_LABEL (self->content_label),
                          "sorry impossible to remove that customer");
      gtk_widget_set_visible (self->info_bar, TRUE);
      sqlite3_finalize(res);
      return;
    }

  buffer = g_strdup_printf ("delete from customer where id is %d",cust_id);
  rc = sqlite3_exec(self->db, buffer, 0, 0, &err_msg);
  g_free(buffer);
  if (rc != SQLITE_OK)
    {
      fprintf(stderr, "SQL error: %s\n", err_msg);

      gtk_label_set_text (GTK_LABEL (self->content_label),
                          "sorry impossible to remove that customer");
      gtk_widget_set_visible (self->info_bar, TRUE);

      return;
    }

  gtk_container_remove(GTK_CONTAINER (list_cust),
                       GTK_WIDGET (row));
  sqlite3_finalize(res);
}

void
edit_cust_cb(PosmanPanelList  *list,
             GtkListBoxRow    *row,
             gchar            *name,
             gchar            *address,
             gchar            *domn_id,
             gchar            *description,
             gchar            *phone,
             gpointer         *user_data)
{
  PosmanWindow *self = POSMAN_WINDOW(user_data);
  CustRowData  *cust_row = g_object_get_data(G_OBJECT(row),"data");
  gchar        *buffer;
  gint          rc;
  sqlite3_stmt *res;
  char         *err_msg = 0;


  if(g_strcmp0(name,gtk_label_get_text (GTK_LABEL (cust_row->cust_name))))
    {
      buffer = g_strdup_printf ("select full_name from customer where full_name = '%s'",name);
      rc = sqlite3_prepare_v2(self->db,buffer,-1, &res, 0);
      g_free(buffer);
      if (rc != SQLITE_OK)
        {
          fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
          sqlite3_finalize(res);
          return;
        }
      sqlite3_step(res);
      if(sqlite3_column_text (res, 0) != NULL)
        {
          gtk_label_set_text (GTK_LABEL (self->content_label),
                              "sorry that name is already exist");
          gtk_widget_set_visible (self->info_bar, TRUE);
          sqlite3_finalize(res);
          return;
        }
      sqlite3_finalize(res);
    }
  buffer = g_strdup_printf ("UPDATE customer SET full_name='%s',address='%s',"\
                            "domain_id=%s,description='%s',phone='%s' "\
                            "WHERE id= %d",
                            name,address,domn_id,description,phone,cust_row->cust_id);
  rc = sqlite3_exec(self->db, buffer, 0, 0, &err_msg);
  g_free(buffer);
  if (rc != SQLITE_OK)
    {
      fprintf(stderr, "SQL error: %s\n", err_msg);

      gtk_label_set_text (GTK_LABEL (self->content_label),
                          "sorry impossible to edit that customer information");
      gtk_widget_set_visible (self->info_bar, TRUE);

      return;
    }
  gtk_label_set_text (GTK_LABEL (cust_row->cust_name),name);
  posman_panel_list_set_view (list,posman_panel_list_main);
}

void
load_edit_cust_cb(PosmanPanelList  *list,
                  GtkListBoxRow    *row,
                  gpointer         *user_data)
{
  PosmanWindow *self = POSMAN_WINDOW(user_data);
  CustRowData  *row_data = g_object_get_data(G_OBJECT(row),"data");
  gchar        *buffer;
  gint          rc;
  sqlite3_stmt *res;

  buffer = g_strdup_printf ("select id,full_name,address,domain_id,description"\
                            ",phone from customer where id is %d",row_data->cust_id);
  rc = sqlite3_prepare_v2(self->db,buffer,-1, &res, 0);
  g_free(buffer);
  if (rc != SQLITE_OK)
    {
      fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
      sqlite3_finalize(res);
      return;
    }
  sqlite3_step(res);
  posman_panel_list_load_edit_cust(list,
                                   row,
                                   sqlite3_column_text (res,1),
                                   sqlite3_column_text (res,2),
                                   sqlite3_column_text (res,3),
                                   sqlite3_column_text (res,4),
                                   sqlite3_column_text (res,5));
  sqlite3_finalize(res);
}

void
load_cust_cb(PosmanPanelList  *list,
             GtkListBoxRow    *row,
             gpointer         *user_data)
{
  PosmanWindow   *self = POSMAN_WINDOW(user_data);
  CustRowData    *row_data = g_object_get_data(G_OBJECT(row),"data");

  /*if (posman_panel_list_get_cust_id(list) != row_data->cust_id)
    {*/

      gchar          *buffer,*pay_buffer;
      gint            rc,pay_rc;
      sqlite3_stmt   *res,*pay_res;

      buffer = g_strdup_printf ("select o.id,o.cust_id,o.status,o.order_date,"\
                                "sum(oi.quantity*oi.price) from orders as o "\
                                "left JOIN order_items as oi on o.id = "\
                                "oi.order_id where o.cust_id = %d group by o.id",row_data->cust_id);

      rc = sqlite3_prepare_v2(self->db,buffer,-1, &res, 0);
      g_free(buffer);
      if (rc != SQLITE_OK)
        {
          fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
          sqlite3_finalize(res);
          return;
        }

      posman_panel_list_set_cust_id(POSMAN_PANEL_LIST (list),row_data->cust_id);
      posman_panel_list_clear(list);

      while (sqlite3_step(res) == SQLITE_ROW)
        {
          CmndRowData   *cmnd_data;
          PosmanAddCmnd *add_cmnd;

          pay_buffer = g_strdup_printf ("select id,sum(val_value),order_id from validation "\
                                        "where order_id = %d group by order_id",
                                        sqlite3_column_int(res,0));
          pay_rc = sqlite3_prepare_v2(self->db,pay_buffer,-1, &pay_res, 0);
          g_free(pay_buffer);
          if (pay_rc != SQLITE_OK)
            {
              fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
              sqlite3_finalize(pay_res);
              return;
            }
          sqlite3_step(pay_res);
          cmnd_data = row_cmnd_data_new(sqlite3_column_int(res,0),
                                        sqlite3_column_text(res,3),
                                        sqlite3_column_double(res,4)
                                        -sqlite3_column_double(pay_res,1));

          add_cmnd = posman_window_cmnd_add_is_active(self,cmnd_data->cmnd_id);

          if(add_cmnd)
            gtk_widget_set_visible (GTK_WIDGET(cmnd_data->in_add), TRUE);

          posman_panel_list_insert_cmnd(list,cmnd_data->row);
          sqlite3_finalize(pay_res);
        }
      sqlite3_finalize(res);
    /*}*/

  posman_panel_list_set_view(list,posman_panel_list_cust);
}

void
clear_items(GtkWidget *widget,
            gpointer data)
{
  gtk_container_remove(GTK_CONTAINER (data),
                       widget);
}

void
load_items_cb(PosmanAddCmnd    *add_cmnd,
              gint              cat_id,
              gpointer          user_data)
{
  PosmanWindow      *self = POSMAN_WINDOW(user_data);
  GtkFlowBox        *items_flow= posman_add_cmnd_get_item_flow (add_cmnd);
  g_autofree gchar  *buffer;
  gint               rc;
  sqlite3_stmt      *res;
  g_autofree gchar  *str_cat_id = g_new(gchar,G_ASCII_DTOSTR_BUF_SIZE);
  str_cat_id = (cat_id == -1)?NULL:g_ascii_dtostr(str_cat_id,sizeof (str_cat_id),cat_id);

  gtk_container_foreach(GTK_CONTAINER (items_flow),
                        clear_items,
                        items_flow);

  buffer = g_strdup_printf ("select id,name,false as type from category "\
                            "where cat_id is %s union "\
                            "select p.id,p.full_name, true as type from stock as s "\
                            "inner join product as p on s.prod_id = p.id and p.category_id is %s "\
                            "left JOIN order_items as o on s.id = o.stock_id group by s.id "\
                            "having sum(o.quantity) < sum(s.quantity) or o.id is null order by type,id",
                            str_cat_id,str_cat_id);

  rc = sqlite3_prepare_v2(self->db,buffer,-1, &res, 0);
  if (rc != SQLITE_OK)
    {
      fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
      sqlite3_finalize(res);
      return;
    }
  while (sqlite3_step(res) != SQLITE_DONE)
    {
      if (sqlite3_column_int(res,2))
        gtk_flow_box_insert(items_flow,(GtkWidget*)
                            posman_item_new (sqlite3_column_text (res,1),
                                             NULL,
                                             sqlite3_column_int(res,0)),-1);
      else
        gtk_flow_box_insert(items_flow,(GtkWidget*)
                            posman_item_new_category (sqlite3_column_text (res,1),
                                                      NULL,
                                                      sqlite3_column_int(res,0)),-1);
    }
  sqlite3_finalize(res);
}

void
item_activated_cb(PosmanAddCmnd    *add_cmnd,
                  GtkFlowBoxChild  *child,
                  GtkListBox       *list,
                  gint              cmnd_domain,
                  gpointer          user_data)
{
   enum{
    price,
    stock_id,
    prodect_name,/**/
    unit_in_pack,/**/
    inc_unit,
    property,/**/
    buy_price,
    rest
  };

  PosmanWindow      *self = POSMAN_WINDOW(user_data);
  g_autofree gchar  *buffer;
  gint               rc;
  sqlite3_stmt      *res;
  PosmanItemListRow *list_row;

  buffer = g_strdup_printf ("WITH RECURSIVE domain_family(id,parent_domain_id) AS ( "\
                            "select id,parent_domain_id from domains where id is %d UNION ALL "\
                            "select d.id,d.parent_domain_id from domains as d,domain_family as f "\
                            "where d.id = f.parent_domain_id) select p.value as price, "\
                            "p.stock_id,pr.full_name  as prodect_name,pr.unit_in_pack,pr.inc_unit,pr.property "\
                            ",s.buy_price,ifnull((s.quantity - sum(o.quantity)),s.quantity) as rest from domains as d1 "\
                            "left join domains as d2 on d2.parent_domain_id = d1.id "\
                            "inner join prices as p on p.domain_id = d1.id "\
                            "inner join product as pr on pr.id = s.prod_id "\
                            "inner join stock as s on s.id = p.stock_id and s.id is %d "\
                            "left JOIN order_items as o on s.id = o.stock_id "\
                            "where domain_id in (SELECT id FROM domain_family)"\
                            "and (d2.id is null or d1.parent_domain_id is null) group by s.id "\
                            "having max(ifnull(d1.parent_domain_id,0.5)) "\
                            "and (sum(o.quantity) < s.quantity or o.id is null) ORDER BY rest desc",
                            cmnd_domain,posman_item_get_id (POSMAN_ITEM (child)));

  rc = sqlite3_prepare_v2(self->db,buffer,-1, &res, 0);
  if (rc != SQLITE_OK)
    {
      fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
      sqlite3_finalize(res);
      return;
    }
  sqlite3_step(res);
  list_row = posman_item_list_row_new (posman_item_get_id (POSMAN_ITEM (child)),
                                       gtk_adjustment_new (sqlite3_column_double (res, price),
                                                           sqlite3_column_double (res, buy_price),
                                                           G_MAXDOUBLE,
                                                           0.5,0.5,0.5),
                                       gtk_adjustment_new (sqlite3_column_double (res, inc_unit),
                                                           sqlite3_column_double (res, inc_unit),
                                                           sqlite3_column_double (res, rest),
                                                           sqlite3_column_double (res, inc_unit),
                                                           1,0),
                                       sqlite3_column_int (res, unit_in_pack),
                                       sqlite3_column_text (res, prodect_name),
                                       -1);

  posman_add_cmnd_add_item(POSMAN_ADD_CMND (add_cmnd),list_row);

  sqlite3_finalize(res);
}

static void
load_stock_cb(PosmanAddCmnd    *add_cmnd,
                  GtkFlowBox       *flow_box,
                  gint              item_id,
                  gint              domain_id,
                  gpointer          user_data)
{
  PosmanWindow      *self = POSMAN_WINDOW(user_data);
  g_autofree gchar  *buffer;
  gint               rc;
  sqlite3_stmt      *res;

  gtk_container_foreach(GTK_CONTAINER (flow_box),
                        clear_items,
                        flow_box);

  buffer = g_strdup_printf ("WITH RECURSIVE domain_family(id,parent_domain_id) AS ( "\
                            "select id,parent_domain_id from domains where id is %d UNION ALL "\
                            "select d.id,d.parent_domain_id from domains as d,domain_family as f "\
                            "where d.id = f.parent_domain_id) "\
                            "select p.value as price,DATE(s.stock_date) as stock_date, "\
                            "p.stock_id,pr.full_name  as prodect_name,pr.unit_in_pack,pr.inc_unit,pr.property "\
                            ",s.buy_price,ifnull((s.quantity - sum(o.quantity)),s.quantity) as rest from domains as d1 "\
                            "left join domains as d2 on d2.parent_domain_id = d1.id "\
                            "inner join prices as p on p.domain_id = d1.id "\
                            "inner join product as pr on pr.id = s.prod_id and pr.id is %d "\
                            "inner join stock as s on s.id = p.stock_id "\
                            "left JOIN order_items as o on s.id = o.stock_id "\
                            "where domain_id in (SELECT id FROM domain_family)"\
                            "and (d2.id is null or d1.parent_domain_id is null) group by s.id "\
                            "having max(ifnull(d1.parent_domain_id,0.5)) "\
                            "and (sum(o.quantity) < s.quantity or o.id is null) ORDER BY rest desc",
                            domain_id,item_id);

  rc = sqlite3_prepare_v2(self->db,buffer,-1, &res, 0);
  if (rc != SQLITE_OK)
    {
      fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
      sqlite3_finalize(res);
      return;
    }
  while (sqlite3_step(res) != SQLITE_DONE)
    {
      gtk_flow_box_insert(flow_box,(GtkWidget*)
                          posman_item_new_stock (sqlite3_column_text (res,3),NULL,
                                                 sqlite3_column_double(res,0),
                                                 sqlite3_column_int(res,8),
                                                 sqlite3_column_int(res,2)),-1);
    }
  posman_add_cmnd_set_stock_visible(POSMAN_ADD_CMND (add_cmnd),TRUE);
  sqlite3_finalize(res);
}

static void
add_cmnd_hold_cb(PosmanAddCmnd    *add_cmnd,
                 GtkListBox       *list_box,
                 gpointer          user_data)
{
  PosmanWindow      *self = POSMAN_WINDOW(user_data);
  GList             *list = gtk_container_get_children(GTK_CONTAINER (list_box));
  gchar        *buffer;
  gint          rc;
  char         *err_msg = 0;

  GList *l = list;
  while (l != NULL)
    {
      if (posman_item_list_get_in_order_id(POSMAN_ITEM_LIST_ROW (l->data)) == -1)
        {
          buffer = g_strdup_printf ("INSERT INTO order_items (order_id,stock_id,"\
                                    "quantity,price) VALUES (%d,%d,%.2f,%.2f)",
                                    posman_add_cmnd_get_cmnd_id (add_cmnd),
                                    posman_item_list_get_id (POSMAN_ITEM_LIST_ROW (l->data)),
                                    gtk_adjustment_get_value (GTK_ADJUSTMENT (posman_item_list_get_qt (POSMAN_ITEM_LIST_ROW (l->data)))),
                                    gtk_adjustment_get_value (GTK_ADJUSTMENT (posman_item_list_get_price (POSMAN_ITEM_LIST_ROW (l->data)))));
          rc = sqlite3_exec(self->db, buffer, 0, 0, &err_msg);
          g_free(buffer);
          if (rc != SQLITE_OK)
            {
              fprintf(stderr, "SQL error: %s\n", err_msg);
              return;
            }
          posman_item_list_set_in_order_id (POSMAN_ITEM_LIST_ROW (l->data),
                                            sqlite3_last_insert_rowid(self->db));
        }
      else
        {
          buffer = g_strdup_printf ("UPDATE order_items SET quantity = %.2f ,price = %.2f WHERE id = %d",
                                    gtk_adjustment_get_value (GTK_ADJUSTMENT (posman_item_list_get_qt (POSMAN_ITEM_LIST_ROW (l->data)))),
                                    gtk_adjustment_get_value (GTK_ADJUSTMENT (posman_item_list_get_price (POSMAN_ITEM_LIST_ROW (l->data)))),
                                    posman_item_list_get_in_order_id(POSMAN_ITEM_LIST_ROW (l->data)));
          rc = sqlite3_exec(self->db, buffer, 0, 0, &err_msg);
          g_free(buffer);
          if (rc != SQLITE_OK)
            {
              fprintf(stderr, "SQL error: %s\n", err_msg);
              return;
            }
        }
      l = l->next;
    }

  posman_panel_list_reload_cust(POSMAN_PANEL_LIST (self->panel_list));
}

static void
add_cmnd_load_selected_cb(PosmanAddCmnd   *add_cmnd,
                          gpointer         user_data)
{
  PosmanWindow      *self = POSMAN_WINDOW(user_data);
  PosmanItemListRow *list_row;
  g_autofree gchar  *buffer;
  gint               rc;
  sqlite3_stmt      *res;

  buffer = g_strdup_printf ("select s.id,s.buy_price,o.id, o.quantity,o.price,"\
                            "p.full_name,p.unit_in_pack,p.inc_unit from stock as s "\
                            "inner join order_items as o on o.stock_id = s.id "\
                            "inner join product as p on s.prod_id = p.id where order_id = %d",
                            posman_add_cmnd_get_cmnd_id (add_cmnd));

  rc = sqlite3_prepare_v2(self->db,buffer,-1, &res, 0);
  if (rc != SQLITE_OK)
    {
      fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
      sqlite3_finalize(res);
      return;
    }
  while (sqlite3_step(res) != SQLITE_DONE)
    {
      g_autofree gchar  *buffer;
      gint               rc;
      sqlite3_stmt      *res_;

      buffer = g_strdup_printf ("select s.quantity-sum(o.quantity) from stock as s "\
                                "inner join order_items as o on o.stock_id = s.id "\
                                "group by stock_id having s.id = %d",
                                sqlite3_column_int (res, 0));

      rc = sqlite3_prepare_v2(self->db,buffer,-1, &res_, 0);
      sqlite3_step(res_);
      if (rc != SQLITE_OK)
        {
          fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
          sqlite3_finalize(res_);
          return;
        }

      list_row = posman_item_list_row_new (sqlite3_column_int (res, 0),
                                           gtk_adjustment_new (sqlite3_column_double (res, 4),
                                                               sqlite3_column_double (res, 1),
                                                               G_MAXDOUBLE,
                                                               0.5,0.5,0.5),
                                           gtk_adjustment_new (sqlite3_column_double (res, 3),
                                                               sqlite3_column_double (res, 7),
                                                               sqlite3_column_double (res_, 0)
                                                               +sqlite3_column_double (res, 3),
                                                               sqlite3_column_double (res, 7),
                                                               1,0),
                                           sqlite3_column_int (res, 6),
                                           sqlite3_column_text (res, 5),
                                           sqlite3_column_int (res, 2));

      sqlite3_finalize(res_);
      posman_add_cmnd_add_item(POSMAN_ADD_CMND (add_cmnd),list_row);
    }
  sqlite3_finalize(res);
}

void
load_cmnd_cb(PosmanPanelList  *list,
             gpointer          user_data)
{
  PosmanWindow      *self = POSMAN_WINDOW(user_data);
  GtkListBoxRow     *row = posman_panel_list_get_selected_cmnd(list);
  CmndRowData       *row_cmnd = g_object_get_data (G_OBJECT(row), "data");
  PosmanAddCmnd     *add_cmnd = posman_window_cmnd_add_is_active(self,row_cmnd->cmnd_id);
  g_autofree  gchar *cmnd_id = g_strdup_printf ("%d",row_cmnd->cmnd_id);

  if(!add_cmnd)
    {
      add_cmnd = posman_add_cmnd_new (row_cmnd->cmnd_id,
                                      posman_panel_list_get_cust_domain(list));
      g_signal_connect (add_cmnd,      "load-items",G_CALLBACK (load_items_cb), self);
      g_signal_connect (add_cmnd,  "item-activated",G_CALLBACK (item_activated_cb), self);
      g_signal_connect (add_cmnd,      "load-stock",G_CALLBACK (load_stock_cb), self);
      g_signal_connect (add_cmnd,          "cancel",G_CALLBACK (gtk_widget_destroy), self);
      g_signal_connect_after (add_cmnd,      "hold",G_CALLBACK (add_cmnd_hold_cb), self);
      g_signal_connect (add_cmnd,   "load-selected",G_CALLBACK (add_cmnd_load_selected_cb), self);
      posman_add_cmnd_load(add_cmnd);
      gtk_stack_add_titled (GTK_STACK(self->action_area_container),
                           GTK_WIDGET(add_cmnd),
                           cmnd_id,cmnd_id);

      gtk_widget_set_visible (GTK_WIDGET(row_cmnd->in_add), TRUE);
      gtk_stack_set_visible_child(GTK_STACK(self->action_area_container),
                                  GTK_WIDGET(add_cmnd));
    }
  else
    gtk_stack_set_visible_child(GTK_STACK(self->action_area_container),
                                GTK_WIDGET(add_cmnd));
}

void
add_cmnd_cb(PosmanPanelList  *list,
            gpointer          user_data)
{
  PosmanWindow      *self = POSMAN_WINDOW(user_data);
  GtkListBoxRow     *row_cust;
  CustRowData       *row;
  g_autofree gchar  *buffer,*cmnd_buffer;
  gint               rc,cmnd_rc;
  char              *err_msg = 0;
  sqlite3_stmt      *cmnd_res;
  GtkWidget         *row_cmnd;


  row_cust = posman_panel_list_get_selected_cust(list);
  row = g_object_get_data (G_OBJECT (row_cust), "data");

  buffer = g_strdup_printf ("insert into orders (cust_id) values (%d)",
                            row->cust_id);
  rc = sqlite3_exec(self->db, buffer, 0, 0, &err_msg);
  if (rc != SQLITE_OK)
    {
      fprintf(stderr, "SQL error: %s\n", err_msg);
      return;
    }
  cmnd_buffer = g_strdup_printf ("select id,order_date from orders where id is %lld",
                                 sqlite3_last_insert_rowid(self->db));
  cmnd_rc = sqlite3_prepare_v2(self->db,cmnd_buffer,-1, &cmnd_res, 0);
  if (cmnd_rc != SQLITE_OK)
    {
      fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
      sqlite3_finalize(cmnd_res);
      return;
    }
  sqlite3_step(cmnd_res);
  row_cmnd = row_cmnd_data_new(sqlite3_column_int(cmnd_res,0),
                               sqlite3_column_text(cmnd_res,1),
                               0.0)->row;
  posman_panel_list_insert_cmnd(list,row_cmnd);
  posman_panel_list_select_cmnd(list,GTK_LIST_BOX_ROW (row_cmnd));

  load_cmnd_cb(list,user_data);
}

static void
remove_cmnd_cb(PosmanPanelList  *list,
               GtkListBoxRow    *row,
               gpointer          user_data)
{
  PosmanWindow      *self = POSMAN_WINDOW(user_data);
  CmndRowData       *row_cmnd = g_object_get_data (G_OBJECT(row), "data");
  g_autofree gchar  *buffer;
  gint               rc;
  char              *err_msg = 0;
  sqlite3_stmt      *res;
  GtkWidget         *add_cmnd;

  buffer = g_strdup_printf ("SELECT status FROM orders where id is %d",
                            row_cmnd->cmnd_id);

  rc = sqlite3_prepare_v2(self->db,buffer,-1, &res, 0);
  if (rc != SQLITE_OK)
    {
      fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
      sqlite3_finalize(res);
      return;
    }
  sqlite3_step(res);
  if (sqlite3_column_int (res,0) == 1)
    posman_window_Notify(self,"sorry impossible to remove that order 'case it's shepped");
  else
    {
      g_free(buffer);
      buffer = g_strdup_printf ("DELETE FROM orders WHERE id is %d;"\
                                "DELETE FROM order_items WHERE order_id is %d;",
                                row_cmnd->cmnd_id,row_cmnd->cmnd_id);
      rc = sqlite3_exec(self->db, buffer, 0, 0, &err_msg);
      if (rc != SQLITE_OK)
        {
          fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(self->db));
          sqlite3_finalize(res);
          return;
        }
      posman_panel_list_remove_cmnd(list,GTK_WIDGET (row));

      add_cmnd = (GtkWidget*)posman_window_cmnd_add_is_active(self,row_cmnd->cmnd_id);
      if (add_cmnd)
        gtk_container_remove (GTK_CONTAINER (self->action_area_container),
                              GTK_WIDGET(add_cmnd));
    }
  sqlite3_finalize(res);
}

static void
gtk_widget_set_visible_cb(GtkWidget *object,
                          gpointer   user_data)
{
  gtk_widget_set_visible(GTK_WIDGET(object),FALSE);
}

static void
add_action_cmnd_cb(GtkContainer *container,
                   GtkWidget    *widget,
                   gpointer      user_data)
{
  PosmanWindow          *self = POSMAN_WINDOW(user_data);
  PosmanActionControler *controler;
  gint                   cmnd_id;
  g_autofree gchar      *str_cmnd_id;
  GtkWidget             *cmnd_row;
  CmndRowData           *cmnd_data;

  cmnd_id = posman_add_cmnd_get_cmnd_id(POSMAN_ADD_CMND (widget));
  str_cmnd_id = g_strdup_printf("%d",cmnd_id);

  controler = posman_action_controler_new (str_cmnd_id,cmnd_id);
  g_signal_connect_swapped (controler, "destroy", G_CALLBACK(gtk_widget_destroy), widget);
  g_signal_connect_swapped (widget, "destroy", G_CALLBACK(gtk_widget_destroy), controler);
  gtk_list_box_prepend(GTK_LIST_BOX (self->action_listbox),
                       GTK_WIDGET (controler));

  cmnd_row = posman_panel_list_get_cmnd_row(POSMAN_PANEL_LIST (self->panel_list),cmnd_id);
  cmnd_data = g_object_get_data(G_OBJECT(cmnd_row),"data");
  g_signal_connect_swapped (widget, "destroy", G_CALLBACK(gtk_widget_set_visible_cb), cmnd_data->in_add);
}

void
action_listbox_row_activated_cb(GtkListBox    *box,
                                GtkListBoxRow *row,
                                gpointer       user_data)
{
  PosmanWindow *self = POSMAN_WINDOW(user_data);
  const gchar  *name = posman_action_controler_get_name (POSMAN_ACTION_CONTROLER (row));
  gtk_stack_set_visible_child_name(GTK_STACK (self->action_area_container),
                                   name);
}

/* object vfonc */

static void posman_window_finalize(GObject *object)
{
  PosmanWindow *self = POSMAN_WINDOW(object);

  sqlite3_close(self->db);

  G_OBJECT_CLASS(posman_window_parent_class)->finalize(object);
}

static void posman_window_constructed(GObject *object)
{
  PosmanWindow *self = POSMAN_WINDOW(object);

  G_OBJECT_CLASS(posman_window_parent_class)->constructed(object);

  g_signal_connect (self->panel_list,      "notify::view",G_CALLBACK (view_changed_cb),self);
  g_signal_connect (self->panel_list,          "add-cust",G_CALLBACK (add_cust_cb), self);
  g_signal_connect (self->panel_list,       "remove-cust",G_CALLBACK (remove_cust_cb), self);
  g_signal_connect (self->panel_list,         "edit-cust",G_CALLBACK (edit_cust_cb), self);
  g_signal_connect (self->panel_list,    "load-edit-cust",G_CALLBACK (load_edit_cust_cb), self);
  g_signal_connect (self->panel_list,         "load-cust",G_CALLBACK (load_cust_cb), self);
  g_signal_connect (self->panel_list,          "add-cmnd",G_CALLBACK (add_cmnd_cb), self);
  g_signal_connect (self->panel_list,         "load-cmnd",G_CALLBACK (load_cmnd_cb), self);
  g_signal_connect (self->panel_list,       "remove-cmnd",G_CALLBACK (remove_cmnd_cb), self);
  g_signal_connect (self->action_area_container,    "add",G_CALLBACK (add_action_cmnd_cb), self);
  g_signal_connect (self->action_listbox, "row-activated",G_CALLBACK (action_listbox_row_activated_cb), self);
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
  gtk_widget_class_bind_template_child(widget_class, PosmanWindow, select_button);
  gtk_widget_class_bind_template_child(widget_class, PosmanWindow, panel_list);
  gtk_widget_class_bind_template_child(widget_class, PosmanWindow, info_bar);
  gtk_widget_class_bind_template_child(widget_class, PosmanWindow, content_label);
  gtk_widget_class_bind_template_child(widget_class, PosmanWindow, action_area_container);
  gtk_widget_class_bind_template_child(widget_class, PosmanWindow, action_listbox);

  gtk_widget_class_bind_template_callback (widget_class, previous_button_cb);
  gtk_widget_class_bind_template_callback (widget_class, select_button_cb);

  g_type_ensure(POSMAN_TYPE_PANEL_LIST);
  g_type_ensure(POSMAN_TYPE_ADD_CMND);
}

static void
posman_window_init(PosmanWindow *self)
{
  gtk_widget_init_template(GTK_WIDGET(self));
  posman_window_init_database(self);
  posman_window_init_cust(self,POSMAN_PANEL_LIST (self->panel_list));
}

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

static PosmanAddCmnd *
posman_window_cmnd_add_is_active(PosmanWindow *self,gint  cmnd_id)
{
  GList   *list = gtk_container_get_children(GTK_CONTAINER (self->action_area_container));

  GList *l = list;
  while (l != NULL)
    {

      if (posman_add_cmnd_get_cmnd_id (l->data) == cmnd_id)
        return  l->data;
      l = l->next;
    }

  return NULL;
}

static void
posman_window_Notify(PosmanWindow *self,const gchar *info)
{
  gtk_label_set_text (GTK_LABEL (self->content_label),
                      info);
  gtk_widget_set_visible (self->info_bar, TRUE);
}
