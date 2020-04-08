#include "posman-add-cmnd.h"
#include "posman-item-chooser.h"
#include "posman-item-list-row.h"
#include "posman-item.h"

struct _PosmanAddCmnd
{
  GtkBox        parent_instance;

  GtkWidget    *list_box_items;
  GtkWidget    *item_chooser;
  GtkWidget    *items;
  GtkWidget    *check_out;
  GtkWidget    *cancel;
  GtkWidget    *hold;
  GtkWidget    *total;

  gint          cmnd_id;
  gint          cmnd_domain;
  gchar        *cmnd_name;
};

G_DEFINE_TYPE (PosmanAddCmnd, posman_add_cmnd, GTK_TYPE_BOX)

enum {
  PROP_0,
  PROP_ITEM,
  PROP_STOCK,
  PROP_SELECTED,
  PROP_CMND_ID,
  PROP_CMND_DOMAIN,
  PROP_CMND_NAME,
  N_PROPS
};

enum {
  LOAD_ITEMS,
  LOAD_STOCK,
  LOAD_SELECTED,
  ITEM_ACTIVATED,
  CANCEL,
  HOLD,

  LAST_SIGNAL
};

static GParamSpec *properties [N_PROPS];
static guint       signals[LAST_SIGNAL];

/**/

gboolean
selected_item_ForeachFunc(GtkFlowBoxChild *child,
                          gpointer user_data)
{
  PosmanAddCmnd *self = (PosmanAddCmnd *)user_data;

  if (posman_add_cmnd_get_list_row(self,posman_item_get_id (POSMAN_ITEM (child))))
    {
      posman_item_set_selected (POSMAN_ITEM (child), TRUE);
      return TRUE;
    }
  posman_item_set_selected (POSMAN_ITEM (child), FALSE);


  return TRUE;
}

gboolean
selected_stock_ForeachFunc(GtkFlowBoxChild *child,
                           gpointer user_data)
{
  PosmanAddCmnd *self = (PosmanAddCmnd *)user_data;

  if (posman_add_cmnd_get_list_row(self,posman_item_get_id (POSMAN_ITEM (child))))
    {
      posman_item_set_selected (POSMAN_ITEM (child), TRUE);
      return TRUE;
    }
  posman_item_set_selected (POSMAN_ITEM (child), FALSE);

  return TRUE;
}

/*callback*/

void
item_chooser_activated_cb(PosmanItemChooser   *item_chooser,
                          GtkFlowBox          *box,
                          GtkFlowBoxChild     *child,
                          gpointer            user_data)
{
  PosmanAddCmnd *self = (PosmanAddCmnd *)user_data;
  if (posman_item_get_item_type(POSMAN_ITEM (child)) == posman_type_item_category)
    g_signal_emit (self, signals[LOAD_ITEMS],0,
                   posman_item_get_id(POSMAN_ITEM (child)));
  else if (posman_item_get_item_type(POSMAN_ITEM (child)) == posman_type_item_item)
    g_signal_emit (self, signals[LOAD_STOCK],0,
                   posman_item_chooser_get_stock_flow(POSMAN_ITEM_CHOOSER (self->item_chooser)),
                   posman_item_get_id(POSMAN_ITEM (child)),
                   self->cmnd_domain);
}

void
stock_chooser_activated_cb(PosmanItemChooser   *item_chooser,
                           GtkFlowBox          *box,
                           GtkFlowBoxChild     *child,
                           gpointer            user_data)
{
  PosmanAddCmnd       *self = (PosmanAddCmnd *)user_data;
  PosmanItemListRow   *list_row = posman_add_cmnd_get_list_row(self,posman_item_get_id (POSMAN_ITEM(child)));
  if (list_row)
    {
      gtk_widget_grab_focus(GTK_WIDGET (list_row));
      return;
    }
  g_signal_emit (self, signals[ITEM_ACTIVATED],0,
                 child,
                 self->list_box_items,
                 self->cmnd_domain);
}

void
go_home_cb(PosmanItemChooser   *item_chooser,
           gpointer             user_data)
{
  PosmanAddCmnd *self = (PosmanAddCmnd *)user_data;
  g_signal_emit (self, signals[LOAD_ITEMS],0,-1);
}

void
cancel_button_cb(GtkButton    *button,
                 gpointer     *user_data)
{
  PosmanAddCmnd *self = (PosmanAddCmnd *)user_data;
  g_signal_emit (self, signals[CANCEL],0);
}

static void
hold_button_cb(GtkButton    *button,
               gpointer     *user_data)
{
  PosmanAddCmnd *self = (PosmanAddCmnd *)user_data;
  g_signal_emit (self, signals[HOLD],0,self->list_box_items);
}

static void
list_box_add_remove(PosmanAddCmnd       *self)
{
  GList               *list;
  guint                n_elmnt;
  gdouble              total = 0.0;
  g_autofree  gchar   *buffer = NULL;
  GtkFlowBox          *flow_box = posman_item_chooser_get_stock_flow(POSMAN_ITEM_CHOOSER (self->item_chooser));

  if(flow_box)
    {
      list = gtk_container_get_children(GTK_CONTAINER (self->list_box_items));
      n_elmnt = g_list_length(list);
      buffer = g_strdup_printf ("%u",n_elmnt);
      gtk_label_set_text (GTK_LABEL(self->items),buffer);
      g_free(buffer);
      gtk_flow_box_invalidate_filter(flow_box);
      while(list != NULL)
        {
          total +=gtk_adjustment_get_value (GTK_ADJUSTMENT (posman_item_list_get_qt (POSMAN_ITEM_LIST_ROW (list->data))))
                  *gtk_adjustment_get_value (GTK_ADJUSTMENT (posman_item_list_get_price (POSMAN_ITEM_LIST_ROW (list->data))));
          list = list->next;
        }
      buffer = g_strdup_printf ("%.2lf",total);
      gtk_label_set_text (GTK_LABEL(self->total),buffer);
    }
}

static void
list_box_add_remove_cb(GtkWidget *object,
                       gpointer   user_data)
{
  PosmanAddCmnd *self = (PosmanAddCmnd *)user_data;
  list_box_add_remove(self);
}

/*vfonc*/

PosmanAddCmnd *
posman_add_cmnd_new (gint   cmnd_id,gint    cmnd_domain)
{
  return g_object_new (POSMAN_TYPE_ADD_CMND,
                       "cmnd-id",cmnd_id,
                       "cmnd-domain",cmnd_domain,
                       NULL);
}

static void
posman_add_cmnd_finalize (GObject *object)
{
  PosmanAddCmnd *self = (PosmanAddCmnd *)object;

  if(self->cmnd_name)
    g_free(self->cmnd_name);

  G_OBJECT_CLASS (posman_add_cmnd_parent_class)->finalize (object);
}

static void
posman_add_cmnd_constructed (GObject *object)
{
  PosmanAddCmnd *self = (PosmanAddCmnd *)object;

  G_OBJECT_CLASS (posman_add_cmnd_parent_class)->constructed (object);

  gtk_flow_box_set_filter_func(posman_item_chooser_get_items_flow(POSMAN_ITEM_CHOOSER (self->item_chooser)),
                                selected_item_ForeachFunc,
                                self,NULL);

  gtk_flow_box_set_filter_func(posman_item_chooser_get_stock_flow(POSMAN_ITEM_CHOOSER (self->item_chooser)),
                                selected_stock_ForeachFunc,
                                self,NULL);
  g_signal_connect (self->item_chooser,  "item-activated", G_CALLBACK (item_chooser_activated_cb), self);
  g_signal_connect (self->item_chooser,  "stock-activated",G_CALLBACK (stock_chooser_activated_cb), self);
  g_signal_connect (self->item_chooser,  "go-home",        G_CALLBACK (go_home_cb), self);
}

static void
posman_add_cmnd_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  PosmanAddCmnd *self = POSMAN_ADD_CMND (object);

  switch (prop_id)
    {
      case PROP_ITEM:
        posman_add_cmnd_get_item_flow (self);
        break;
      case PROP_STOCK:
        posman_add_cmnd_get_stock_flow (self);
        break;
      case PROP_SELECTED:
        posman_add_cmnd_get_selected_list (self);
        break;
      case PROP_CMND_ID:
        posman_add_cmnd_get_cmnd_id(self);
        break;
      case PROP_CMND_DOMAIN:
        posman_add_cmnd_get_cmnd_domain(self);
        break;
      case PROP_CMND_NAME:
        posman_add_cmnd_get_cmnd_name(self);
        break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
posman_add_cmnd_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  PosmanAddCmnd *self = POSMAN_ADD_CMND (object);

  switch (prop_id)
    {
    default:
      case PROP_CMND_ID:
        posman_add_cmnd_set_cmnd_id(self,g_value_get_int (value));
        break;
      case PROP_CMND_DOMAIN:
        posman_add_cmnd_set_cmnd_domain(self,g_value_get_int(value));
        break;
      case PROP_CMND_NAME:
        posman_add_cmnd_set_cmnd_name(self,g_value_get_string (value));
        break;
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
posman_add_cmnd_class_init (PosmanAddCmndClass *klass)
{
  GObjectClass    *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = posman_add_cmnd_finalize;
  object_class->constructed = posman_add_cmnd_constructed;
  object_class->get_property = posman_add_cmnd_get_property;
  object_class->set_property = posman_add_cmnd_set_property;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/pos/manager/posman-add-cmnd.ui");

  gtk_widget_class_bind_template_child (widget_class, PosmanAddCmnd, list_box_items);
  gtk_widget_class_bind_template_child (widget_class, PosmanAddCmnd, item_chooser);
  gtk_widget_class_bind_template_child (widget_class, PosmanAddCmnd, items);
  gtk_widget_class_bind_template_child (widget_class, PosmanAddCmnd, check_out);
  gtk_widget_class_bind_template_child (widget_class, PosmanAddCmnd, cancel);
  gtk_widget_class_bind_template_child (widget_class, PosmanAddCmnd, hold);
  gtk_widget_class_bind_template_child (widget_class, PosmanAddCmnd, total);

  gtk_widget_class_bind_template_callback (widget_class, cancel_button_cb);
  gtk_widget_class_bind_template_callback (widget_class, hold_button_cb);

  properties[PROP_ITEM] = g_param_spec_object ("item",
                                               "item",
                                               "items in flow box",
                                               GTK_TYPE_FLOW_BOX,
                                               G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_STOCK] = g_param_spec_object ("stock",
                                                "stock",
                                                "stock in flow box",
                                                GTK_TYPE_FLOW_BOX,
                                                G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_SELECTED] = g_param_spec_object ("selected",
                                                   "selected",
                                                   "items selected in list box",
                                                   GTK_TYPE_LIST_BOX,
                                                   G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_CMND_ID] = g_param_spec_int ("cmnd-id",
                                               "cmnd-id",
                                               "the cmnd id",
                                               -1,G_MAXINT,-1,
                                               G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_CONSTRUCT_ONLY);

  properties[PROP_CMND_DOMAIN] = g_param_spec_int ("cmnd-domain",
                                                   "cmnd-domain",
                                                   "the cmnd domain",
                                                   -1,G_MAXINT,-1,
                                                   G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_CONSTRUCT_ONLY);

  properties[PROP_CMND_NAME] = g_param_spec_string ("cmnd-name",
                                                    "cmnd-name",
                                                    "cmnd name for stack",
                                                    "null",
                                                    G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_CONSTRUCT_ONLY);

  g_object_class_install_properties (object_class, N_PROPS, properties);

  signals[LOAD_ITEMS] = g_signal_new("load-items",
                                     G_OBJECT_CLASS_TYPE (object_class),
                                     G_SIGNAL_RUN_FIRST,
                                     0,
                                     NULL,NULL,
                                     NULL,
                                     G_TYPE_NONE,1,
                                     G_TYPE_INT);
  signals[LOAD_STOCK] = g_signal_new("load-stock",
                                     G_OBJECT_CLASS_TYPE (object_class),
                                     G_SIGNAL_RUN_FIRST,
                                     0,
                                     NULL,NULL,
                                     NULL,
                                     G_TYPE_NONE,3,
                                     GTK_TYPE_FLOW_BOX,
                                     G_TYPE_INT,
                                     G_TYPE_INT);

  signals[LOAD_SELECTED] = g_signal_new("load-selected",
                                        G_OBJECT_CLASS_TYPE (object_class),
                                        G_SIGNAL_RUN_FIRST,
                                        0,NULL,NULL,
                                        NULL,
                                        G_TYPE_NONE,0);
  signals[ITEM_ACTIVATED] = g_signal_new("item-activated",
                                         G_OBJECT_CLASS_TYPE (object_class),
                                         G_SIGNAL_RUN_FIRST,
                                         0,NULL,NULL,NULL,
                                         G_TYPE_NONE,3,
                                         GTK_TYPE_FLOW_BOX_CHILD,
                                         GTK_TYPE_LIST_BOX,
                                         G_TYPE_INT);

  signals[CANCEL] = g_signal_new("cancel",
                                 G_OBJECT_CLASS_TYPE (object_class),
                                 G_SIGNAL_RUN_FIRST,
                                 0,NULL,NULL,NULL,
                                 G_TYPE_NONE,0);

  signals[HOLD] = g_signal_new("hold",
                               G_OBJECT_CLASS_TYPE (object_class),
                               G_SIGNAL_RUN_FIRST,
                               0,NULL,NULL,NULL,
                               G_TYPE_NONE,1,
                               GTK_TYPE_LIST_BOX);

  g_type_ensure (POSMAN_TYPE_ITEM_CHOOSER);
}

static void
posman_add_cmnd_init (PosmanAddCmnd *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

GtkFlowBox *
posman_add_cmnd_get_item_flow(PosmanAddCmnd   *self)
{
  GtkFlowBox *items;

  items = posman_item_chooser_get_items_flow(POSMAN_ITEM_CHOOSER (self->item_chooser));

  return GTK_FLOW_BOX (items);
}

GtkFlowBox *
posman_add_cmnd_get_stock_flow(PosmanAddCmnd   *self)
{
  GtkFlowBox *items;

  items = posman_item_chooser_get_stock_flow(POSMAN_ITEM_CHOOSER (self->item_chooser));

  return GTK_FLOW_BOX (items);
}

GtkListBox *
posman_add_cmnd_get_selected_list(PosmanAddCmnd   *self)
{
  return GTK_LIST_BOX (self->list_box_items);
}

void
posman_add_cmnd_set_cmnd_id(PosmanAddCmnd   *self,gint  cmnd_id)
{
  g_return_if_fail (POSMAN_IS_ADD_CMND (self));

  self->cmnd_id = cmnd_id;

  g_object_notify_by_pspec (G_OBJECT (self),properties[PROP_CMND_ID]);
}

gint
posman_add_cmnd_get_cmnd_id(PosmanAddCmnd   *self)
{
  g_return_val_if_fail (POSMAN_IS_ADD_CMND (self),-1);
  return self->cmnd_id;
}

void
posman_add_cmnd_set_cmnd_domain(PosmanAddCmnd   *self,gint  cmnd_domain)
{
  g_return_if_fail (POSMAN_IS_ADD_CMND (self));

  self->cmnd_domain = cmnd_domain;

  g_object_notify_by_pspec (G_OBJECT (self),properties[PROP_CMND_DOMAIN]);
}

gint
posman_add_cmnd_get_cmnd_domain(PosmanAddCmnd   *self)
{
  g_return_val_if_fail (POSMAN_IS_ADD_CMND (self),-1);
  return self->cmnd_domain;
}

void
posman_add_cmnd_load(PosmanAddCmnd    *self)
{
  g_return_if_fail (POSMAN_IS_ADD_CMND (self));

  g_signal_emit(self,signals[LOAD_ITEMS],0,-1);
  g_signal_emit(self,signals[LOAD_SELECTED],0);
}

const gchar *
posman_add_cmnd_get_cmnd_name(PosmanAddCmnd    *self)
{
  g_return_val_if_fail (POSMAN_IS_ADD_CMND (self),NULL);
  return self->cmnd_name;
}

void
posman_add_cmnd_set_cmnd_name(PosmanAddCmnd    *self,gchar    *name)
{
  g_return_if_fail (POSMAN_IS_ADD_CMND (self));

  self->cmnd_name = g_strdup(name);

  g_object_notify_by_pspec (G_OBJECT (self),properties[PROP_CMND_NAME]);
}

void
posman_add_cmnd_set_stock_visible(PosmanAddCmnd    *self,gboolean   visible)
{
  g_return_if_fail (POSMAN_IS_ADD_CMND (self));

  posman_item_chooser_set_stock_visible(POSMAN_ITEM_CHOOSER (self->item_chooser),
                                        visible);
}

void
posman_add_cmnd_add_item (PosmanAddCmnd    *self,PosmanItemListRow    *row)
{
  g_return_if_fail (POSMAN_IS_ADD_CMND (self));

  gtk_list_box_insert (GTK_LIST_BOX (self->list_box_items), GTK_WIDGET (row), -1);
  list_box_add_remove(self);
  g_signal_connect (row,"destroy",G_CALLBACK (list_box_add_remove_cb), self);
  g_signal_connect (row,"value",  G_CALLBACK (list_box_add_remove_cb), self);
}

PosmanItemListRow *
posman_add_cmnd_get_list_row (PosmanAddCmnd    *self,gint   row_id)
{
  g_return_val_if_fail (POSMAN_IS_ADD_CMND (self),NULL);

  GList   *list = gtk_container_get_children(GTK_CONTAINER (self->list_box_items));

  GList *l = list;
  while (l != NULL)
    {


      if (posman_item_list_get_id (POSMAN_ITEM_LIST_ROW (l->data)) == row_id)
        return  l->data;
      l = l->next;
    }

  return NULL;
}


