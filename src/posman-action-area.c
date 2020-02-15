#include "posman-action-area.h"

struct _PosmanActionArea {
  GtkStack parent_instance;


  GtkWidget             *cmnd_items;
  GtkWidget             *items;
  GtkWidget             *items_viewer;

  GtkWidget             *treeview_items;
  GtkWidget             *iconview_items_viewer;

  GtkWidget             *add_cmnd;
  GtkWidget             *dashboard;
  GtkWidget             *payment_validation;

  GtkWidget             *label1;

  PosmanActionAreaView  view;
  gint64                cust_id;
  gint                  cust_dmn;

  GtkWidget             *price_viewer;
  GtkWidget             *qt_sb;
  GtkWidget             *price_sb;
  GtkWidget             *combo_date;

  /* tree view clom and rend */

  GtkWidget             *qt_colm,*cell_qt;
  GtkWidget             *price_colm,*cell_price;
  GtkWidget             *qt_price_colm,*cell_qt_price;
};

G_DEFINE_TYPE(PosmanActionArea, posman_action_area, GTK_TYPE_STACK)

enum {
  PROP_0,
  PROP_ITEMS,
  PROP_ITEMS_VIEWER,
  PROP_VIEW,
  PROP_CUST_ID,
  PROP_CUST_DMN,
  PROP_ICONVIEW_ITEM_VIEWER,
  PROP_COMBO,

  N_PROPS
};

enum {
  VIEW_CHANGED,
  ITEM_ACTIVATED,
  HOME_ACTIVATED,

  LAST_SIGNAL
};

static GParamSpec *properties [N_PROPS];
static guint      signals[LAST_SIGNAL];

static GtkWidget *
get_action_area_from_view(PosmanActionArea      *self,
                          PosmanActionAreaView  view);

/* callback */

void
item_activated_callback0(GtkIconView *iconview,
                        GtkTreePath *path,
                        gpointer     user_data)
{
  PosmanActionArea  *self = POSMAN_ACTION_AREA (user_data);

  g_signal_emit (self, signals[ITEM_ACTIVATED], 0, iconview, path);
}

void
home_activated_cb(GtkButton *button,
                  gpointer   user_data)
{
  PosmanActionArea  *self = POSMAN_ACTION_AREA (user_data);
  g_signal_emit (self, signals[HOME_ACTIVATED], 0, self->items_viewer);
}

GValue *
gtk_tree_model_get_values(GtkTreeModel  *combo_model,
                          GtkTreeIter   iter)
{
  gint n = gtk_tree_model_get_n_columns(combo_model);
  GValue    *value_array = g_new0 (GValue, n);
  for(gint i = 0; i<n ;i++)
    {
      gtk_tree_model_get_value(combo_model,
                               &iter,
                               i,
                               &value_array[i]);
    }
  return &value_array[0];
}

gboolean
did_we_had_this_stock(GtkTreeModel  *list_model,
                      GValue        *value,
                      GtkTreeIter   *iter_c)
{
  GValue value_check = G_VALUE_INIT;
  if(!gtk_tree_model_get_iter_first(GTK_TREE_MODEL (list_model),iter_c))
    return FALSE;
  do{
  gtk_tree_model_get_value(list_model,
                           iter_c,
                           1,&value_check);
  if(!g_strcmp0 (g_value_get_string (&value_check),g_value_get_string (value)))
      {
        return TRUE;
      }
  g_value_unset (&value_check);
  }while (gtk_tree_model_iter_next(GTK_TREE_MODEL (list_model),iter_c));
  return FALSE;
}

void
value_free(GValue   *value)
{
  for(gint i = 0;i<=6;i++)
    {
      g_value_unset (&value[i]);
    }
}

void
add_button_callback(GtkButton *button,
                    gpointer   user_data)
{
  PosmanActionArea  *self = POSMAN_ACTION_AREA (user_data);
  GtkTreeIter       iter,iter_c;
  GtkTreeModel      *combo_model = gtk_combo_box_get_model (GTK_COMBO_BOX (self->combo_date));
  GtkTreeModel      *list_model = gtk_tree_view_get_model (GTK_TREE_VIEW (self->treeview_items));
  gint              columns[] = {0,1,2,3,4,5,6};

  if(!gtk_tree_model_get_iter_first(GTK_TREE_MODEL (combo_model),
                                    &iter))
    {
      gtk_widget_set_visible (GTK_WIDGET(self->price_viewer),FALSE);
      gtk_tree_model_get_iter_first(list_model,&iter_c);
      gtk_tree_model_row_changed(list_model,
                                     gtk_tree_path_new_first(),
                                     &iter_c);
      return;
    }
  do
    {
  GValue *value = gtk_tree_model_get_values(combo_model,iter);

      if(gtk_adjustment_get_value (GTK_ADJUSTMENT (g_value_get_object (&value[2]))) == 0)
        continue;
      if(did_we_had_this_stock(list_model,
                               &value[1],
                               &iter_c))
        {
          GtkAdjustment   *adj;
          gtk_tree_model_get(list_model,
                             &iter_c,
                             2,&adj,-1);
          gtk_adjustment_set_value(adj,
                                   gtk_adjustment_get_value (adj)
                                   +gtk_adjustment_get_value (g_value_get_object (&value[2])));
          gtk_tree_model_row_changed(list_model,
                                     gtk_tree_model_get_path(list_model,&iter_c),
                                     &iter_c);
          continue;
        }
      gtk_list_store_append(GTK_LIST_STORE (list_model),&iter_c);
      gtk_list_store_set_valuesv(GTK_LIST_STORE (list_model),
                                 &iter_c,
                                 columns,
                                 value,
                                 gtk_tree_model_get_n_columns(combo_model));
      value_free(value);
    }while (gtk_tree_model_iter_next(GTK_TREE_MODEL (combo_model),
                                     &iter));
  gtk_widget_set_visible (GTK_WIDGET(self->price_viewer),
                          FALSE);
}

void
treeview_row_activated_cb(GtkTreeView       *tree_view,
                          GtkTreePath       *path,
                          GtkTreeViewColumn *column,
                          gpointer           user_data)
{
  PosmanActionArea  *self = POSMAN_ACTION_AREA (user_data);
  GtkTreeModel      *model = gtk_tree_view_get_model (tree_view);
  GtkTreeModel      *combo_model = gtk_combo_box_get_model(GTK_COMBO_BOX (self->combo_date));
  GtkTreeIter       iter;
  GtkAdjustment     *p_adj,*q_adj;
  gtk_tree_model_get_iter(model,&iter,path);

  gtk_tree_model_get(model,&iter,2,&q_adj,3,&p_adj,-1);
  gtk_spin_button_set_adjustment (GTK_SPIN_BUTTON(self->price_sb),p_adj);
  gtk_spin_button_set_adjustment (GTK_SPIN_BUTTON(self->qt_sb),q_adj);
  gtk_list_store_clear (GTK_LIST_STORE(combo_model));
  gtk_widget_set_visible (GTK_WIDGET(self->price_viewer),TRUE);
}

void
cancel_button_cb(GtkButton *button,
                 gpointer   user_data)
{
  PosmanActionArea  *self = POSMAN_ACTION_AREA (user_data);
  gtk_list_store_clear (GTK_LIST_STORE (self->items_viewer));
  gtk_list_store_clear (GTK_LIST_STORE (self->items));
  gtk_widget_set_visible(self->price_viewer,FALSE);
  posman_action_area_set_view(self,posman_action_area_view_main);

}

void
treeview_columns_changed_cb(GtkTreeView *tree_view,
                            gpointer     user_data)
{
  PosmanActionArea  *self = POSMAN_ACTION_AREA (user_data);
}

/* Auxiliary */


static GtkWidget *
get_action_area_from_view(PosmanActionArea      *self,
                          PosmanActionAreaView  view)
{
  switch (view)
    {
    case posman_action_area_view_main:
      return self->dashboard;
      break;
    case posman_action_area_view_add_cmnd:
      return self->add_cmnd;
      break;
    case posman_action_area_view_payment_validation:
      return self->payment_validation;
      break;
    default:
      return NULL;
      break;
    }
}

static void
posman_action_area_active_id(GObject    *gobject,
                             GParamSpec *pspec,
                             gpointer    user_data)
{
  GtkComboBox         *combobox = GTK_COMBO_BOX (gobject);
  PosmanActionArea    *self = POSMAN_ACTION_AREA (user_data);
  GtkTreeModel        *model;
  GtkTreeIter         iter;
  GtkWidget           *adj1,*adj2;

  if(gtk_combo_box_get_active_id (combobox) == NULL)
    return;

  model = gtk_combo_box_get_model (combobox);
  gtk_combo_box_get_active_iter(combobox,&iter);
  gtk_tree_model_get (model, &iter,2,&adj1,3,&adj2,-1);

  gtk_spin_button_set_adjustment (GTK_SPIN_BUTTON (self->qt_sb),
                                  GTK_ADJUSTMENT (adj1));
  gtk_spin_button_set_adjustment (GTK_SPIN_BUTTON (self->price_sb),
                                  GTK_ADJUSTMENT (adj2));
}

/* vfnc */

PosmanActionArea *
posman_action_area_new(void)
{
  return g_object_new(POSMAN_TYPE_ACTION_AREA, NULL);
}

static void
posman_action_area_finalize(GObject *object)
{
  /*PosmanActionArea *self = (PosmanActionArea*)object;*/

  G_OBJECT_CLASS(posman_action_area_parent_class)->finalize(object);
}

static void
posman_action_area_dispose(GObject *object)
{
  PosmanActionArea *self = (PosmanActionArea*)object;

  if(self->items)
    g_object_unref (self->items);

  G_OBJECT_CLASS(posman_action_area_parent_class)->dispose(object);
}

static void
posman_action_area_constructed(GObject  *object)
{
  PosmanActionArea  *self = (PosmanActionArea*)object;
  GtkListStore      *model;

  G_OBJECT_CLASS(posman_action_area_parent_class)->constructed(object);

  gtk_label_set_text (GTK_LABEL (self->label1),
                      g_strdup_printf ("%li",self->cust_id));

  self->items = (GtkWidget*)gtk_list_store_new(7,G_TYPE_STRING,
                                               G_TYPE_STRING,
                                               GTK_TYPE_ADJUSTMENT,
                                               GTK_TYPE_ADJUSTMENT,
                                               G_TYPE_STRING,
                                               G_TYPE_STRING,
                                               G_TYPE_STRING);
  gtk_tree_view_set_model (GTK_TREE_VIEW (self->treeview_items),
                           GTK_TREE_MODEL (self->items));


  model = gtk_list_store_new (7,G_TYPE_STRING,
                              G_TYPE_STRING,
                              GTK_TYPE_ADJUSTMENT,
                              GTK_TYPE_ADJUSTMENT,
                              G_TYPE_STRING,
                              G_TYPE_STRING,
                              G_TYPE_STRING);
  gtk_combo_box_set_model (GTK_COMBO_BOX (self->combo_date),
                           GTK_TREE_MODEL (model));

  g_signal_connect (self->combo_date,
                    "notify::active-id",
                    G_CALLBACK (posman_action_area_active_id),
                    object);
}

static void
posman_action_area_get_property(GObject    *object,
                                guint prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  PosmanActionArea *self = POSMAN_ACTION_AREA(object);

  switch (prop_id)
    {
      case PROP_VIEW:
        g_value_set_int(value,self->view);
        break;
      case PROP_ITEMS:
        g_value_set_object(value,self->items);
        break;
      case PROP_ITEMS_VIEWER:
        g_value_set_object (value, self->items_viewer);
        break;
      case PROP_CUST_ID:
        g_value_set_int64(value,self->cust_id);
        break;
      case PROP_ICONVIEW_ITEM_VIEWER:
        g_value_set_object (value, self->iconview_items_viewer);
        break;
      case PROP_COMBO:
        g_value_set_object (value, self->combo_date);
        break;
      case PROP_CUST_DMN:
        g_value_set_int (value,self->cust_dmn);
        break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    }
}

static void
posman_action_area_set_property(GObject      *object,
                                guint prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  PosmanActionArea *self = POSMAN_ACTION_AREA(object);

  switch (prop_id)
    {
      case PROP_VIEW:
        posman_action_area_set_view(self,
                                  g_value_get_int (value));
        break;
      case PROP_CUST_ID:
        posman_action_area_set_cust_id(self,
                                       g_value_get_int64 (value));
        break;
      case PROP_CUST_DMN:
        posman_action_area_set_cust_dmn(self,
                                       g_value_get_int (value));
        break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    }
}


static void
posman_action_area_class_init(PosmanActionAreaClass *klass)
{
  GObjectClass    *object_class = G_OBJECT_CLASS(klass);
  GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS(klass);

  object_class->finalize = posman_action_area_finalize;
  object_class->dispose = posman_action_area_dispose;
  object_class->constructed = posman_action_area_constructed;
  object_class->get_property = posman_action_area_get_property;
  object_class->set_property = posman_action_area_set_property;

  gtk_widget_class_set_template_from_resource(widget_class,
                                              "/org/pos/manager/posman-action-area.ui");

  gtk_widget_class_bind_template_child(widget_class,PosmanActionArea,dashboard);
  gtk_widget_class_bind_template_child(widget_class,PosmanActionArea,add_cmnd);
  gtk_widget_class_bind_template_child(widget_class,PosmanActionArea,payment_validation);
  gtk_widget_class_bind_template_child(widget_class,PosmanActionArea,label1);

  gtk_widget_class_bind_template_child(widget_class,PosmanActionArea,items_viewer);
  gtk_widget_class_bind_template_child(widget_class,PosmanActionArea,treeview_items);
  gtk_widget_class_bind_template_child(widget_class,PosmanActionArea,iconview_items_viewer);
  gtk_widget_class_bind_template_child (widget_class, PosmanActionArea, qt_sb);
  gtk_widget_class_bind_template_child (widget_class, PosmanActionArea, price_sb);
  gtk_widget_class_bind_template_child (widget_class, PosmanActionArea, combo_date);
  gtk_widget_class_bind_template_child (widget_class, PosmanActionArea, qt_colm);
  gtk_widget_class_bind_template_child (widget_class, PosmanActionArea, cell_qt);
  gtk_widget_class_bind_template_child (widget_class, PosmanActionArea, price_colm);
  gtk_widget_class_bind_template_child (widget_class, PosmanActionArea, cell_price);
  gtk_widget_class_bind_template_child (widget_class, PosmanActionArea, price_viewer);
  gtk_widget_class_bind_template_child (widget_class, PosmanActionArea, qt_price_colm);
  gtk_widget_class_bind_template_child (widget_class, PosmanActionArea, cell_qt_price);

  gtk_widget_class_bind_template_callback (widget_class, item_activated_callback0);
  gtk_widget_class_bind_template_callback (widget_class, home_activated_cb);
  gtk_widget_class_bind_template_callback (widget_class, add_button_callback);
  gtk_widget_class_bind_template_callback (widget_class, cancel_button_cb);
  gtk_widget_class_bind_template_callback (widget_class, treeview_row_activated_cb);
  gtk_widget_class_bind_template_callback (widget_class, treeview_columns_changed_cb);

  properties[PROP_ITEMS] =
  g_param_spec_object ("items",
                       "items",
                       "items that i can add",
                       GTK_TYPE_TREE_STORE,
                       G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_VIEW] =
  g_param_spec_int ("view",
                    "view",
                    "customer area view",
                    posman_action_area_view_main,
                    posman_action_area_view_payment_validation,
                    posman_action_area_view_main,
                    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_ITEMS_VIEWER] =
  g_param_spec_object ("items-viewer",
                       "items-viewer",
                       "items in the stock",
                       GTK_TYPE_TREE_STORE,
                       G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_ICONVIEW_ITEM_VIEWER] =
  g_param_spec_object ("iconview-items-viewer",
                       "iconview-items-viewer",
                       "the viewer of items in the stock",
                       GTK_TYPE_ICON_VIEW,
                       G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_CUST_ID] =
  g_param_spec_int64("cust-id",
                     "cust-id",
                     "customer id",
                     -1,
                     G_MAXINT64,
                     -1,
                     G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_CONSTRUCT_ONLY);

  properties[PROP_CUST_DMN] =
  g_param_spec_int("cust-dmn",
                    "cust-dmn",
                    "customer domain",
                    -1,
                    G_MAXINT,
                    -1,
                    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_CONSTRUCT_ONLY);

  properties[PROP_COMBO] =
  g_param_spec_object ("combo-module",
                       "combo-module",
                       "combobox for the stock date and the rest quantity",
                       GTK_TYPE_COMBO_BOX,
                       G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (object_class, N_PROPS, properties);

  signals[VIEW_CHANGED] = g_signal_new("view-changed",
                                       G_OBJECT_CLASS_TYPE (object_class),
                                       G_SIGNAL_RUN_FIRST,
                                       0,
                                       NULL,NULL,
                                       NULL,
                                       G_TYPE_NONE,
                                       0);

  signals[ITEM_ACTIVATED] = g_signal_new("item-activated",
                                         G_OBJECT_CLASS_TYPE(object_class),
                                         G_SIGNAL_RUN_LAST,
                                         0,
                                         NULL,NULL,
                                         NULL,
                                         G_TYPE_NONE,
                                         2,
                                         GTK_TYPE_ICON_VIEW,
                                         GTK_TYPE_TREE_PATH);
  signals[HOME_ACTIVATED] = g_signal_new("home-activated",
                                       G_OBJECT_CLASS_TYPE (object_class),
                                       G_SIGNAL_RUN_FIRST,
                                       0,
                                       NULL,NULL,
                                       NULL,
                                       G_TYPE_NONE,
                                       1,
                                       GTK_TYPE_LIST_STORE);


}

void
cell_update_cb(GtkAdjustment *adjustment,
               gpointer       user_data)
{
  GtkCellRenderer     *cell = GTK_CELL_RENDERER (user_data);

  g_object_set(cell,"text",
               g_strdup_printf ("%.2f",gtk_adjustment_get_value(adjustment)),NULL);
}

void
gtk_tree_cell_data_fonc_price(GtkTreeViewColumn *tree_column,
                              GtkCellRenderer *cell,
                              GtkTreeModel *tree_model,
                              GtkTreeIter *iter,
                              gpointer data)
{
  GtkAdjustment   *adj;
  gtk_tree_model_get (tree_model,
                      iter, 3, &adj, -1);
  g_object_set(cell,"text",
               g_strdup_printf ("%.2f",gtk_adjustment_get_value(adj)),NULL);
}

void
gtk_tree_cell_data_fonc_qt(GtkTreeViewColumn *tree_column,
                           GtkCellRenderer *cell,
                           GtkTreeModel *tree_model,
                           GtkTreeIter *iter,
                           gpointer data)
{
  GtkAdjustment   *adj;
  gtk_tree_model_get (tree_model,
                      iter, 2, &adj, -1);
  g_object_set(cell,"text",
               g_strdup_printf ("%.0f",gtk_adjustment_get_value(adj)),NULL);
}

void
gtk_tree_cell_data_fonc_qt_price(GtkTreeViewColumn *tree_column,
                                 GtkCellRenderer *cell,
                                 GtkTreeModel *tree_model,
                                 GtkTreeIter *iter,
                                 gpointer data)
{
  GtkAdjustment     *adj1,*adj2;
  gdouble           total;
  g_autofree char*  str_total;

  gtk_tree_model_get (tree_model,
                      iter, 2, &adj1,3,&adj2, -1);
  total = gtk_adjustment_get_value(adj1) * gtk_adjustment_get_value(adj2);
  str_total = g_strdup_printf ("%.2f",total);
  g_object_set(cell,"text",str_total,NULL);
}

static void
posman_action_area_init(PosmanActionArea *self)
{
  gtk_widget_init_template(GTK_WIDGET(self));

  gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN (self->price_colm),
                                          GTK_CELL_RENDERER (self->cell_price),
                                          gtk_tree_cell_data_fonc_price,
                                          NULL,NULL);
  gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN (self->qt_colm),
                                          GTK_CELL_RENDERER (self->cell_qt),
                                          gtk_tree_cell_data_fonc_qt,
                                          NULL,NULL);
  gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN (self->qt_price_colm),
                                          GTK_CELL_RENDERER (self->cell_qt_price),
                                          gtk_tree_cell_data_fonc_qt_price,
                                          NULL,NULL);
}

void
posman_action_area_set_view(PosmanActionArea      *self,
                            PosmanActionAreaView  view)
{
  g_return_if_fail (POSMAN_IS_ACTION_AREA (self));

  if (self->view == view)
    return;

  self->view = view;
  gtk_stack_set_visible_child (GTK_STACK(self),
                               get_action_area_from_view (self, view));

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_VIEW]);
}

PosmanActionAreaView
posman_action_area_get_view(PosmanActionArea  *self)
{
  g_return_val_if_fail (POSMAN_IS_ACTION_AREA (self),-1);

  return self->view;
}

GtkWidget *
posman_action_area_get_items_viewer(PosmanActionArea  *self)
{
  g_return_val_if_fail (POSMAN_IS_ACTION_AREA (self),NULL);

  return self->items_viewer;
}

GtkWidget *
posman_action_area_get_items(PosmanActionArea *self)
{
  g_return_val_if_fail (POSMAN_IS_ACTION_AREA (self),NULL);

  return self->items;
}

void
posman_action_area_set_cust_id(PosmanActionArea  *self,
                               gint64            cust_id)
{
  g_return_if_fail (POSMAN_IS_ACTION_AREA (self));

  if(cust_id >= 0)
    self->cust_id = cust_id;

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_CUST_ID]);
}

void
posman_action_area_set_cust_dmn(PosmanActionArea  *self,
                                gint              cust_dmn)
{
  g_return_if_fail (POSMAN_IS_ACTION_AREA (self));

  if(cust_dmn >= 0)
    self->cust_dmn = cust_dmn;

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_CUST_DMN]);
}

gint64
posman_action_area_get_cust_id(PosmanActionArea *self)
{
  g_return_val_if_fail (POSMAN_IS_ACTION_AREA (self),-1);

  return self->cust_id;
}

gint
posman_action_area_get_cust_dmn(PosmanActionArea *self)
{
  g_return_val_if_fail (POSMAN_IS_ACTION_AREA (self),-1);

  return self->cust_dmn;
}

GtkIconView *
posman_action_area_get_iconview_item_viewer(PosmanActionArea  *self)
{
  g_return_val_if_fail (POSMAN_IS_ACTION_AREA (self),NULL);

  return (GtkIconView*) self->iconview_items_viewer;
}

void
posman_action_area_set_qt_adj(PosmanActionArea  *self,GtkAdjustment *adj)
{
  g_return_if_fail (POSMAN_IS_ACTION_AREA (self));
  g_return_if_fail (GTK_IS_ADJUSTMENT (adj));

  gtk_spin_button_set_adjustment (GTK_SPIN_BUTTON (self->qt_sb), adj);
}

void
posman_action_area_set_price_adj(PosmanActionArea  *self,GtkAdjustment *adj)
{
  g_return_if_fail (POSMAN_IS_ACTION_AREA (self));
  g_return_if_fail (GTK_IS_ADJUSTMENT (adj));

  gtk_spin_button_set_adjustment (GTK_SPIN_BUTTON (self->price_sb), adj);
}

GtkComboBox*
posman_action_area_get_combo(PosmanActionArea  *self)
{
  return  GTK_COMBO_BOX (self->combo_date);
}

void
posman_action_area_set_visible(PosmanActionArea  *self,
                               gboolean          visible)
{
  g_return_if_fail (POSMAN_IS_ACTION_AREA (self));
  gtk_widget_set_visible(self->price_viewer,visible);
}
