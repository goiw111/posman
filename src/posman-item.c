#include "posman-item.h"
#include <gdk-pixbuf/gdk-pixbuf.h>

struct _PosmanItem
{
  GtkFlowBoxChild parent_instance;


  posmanitemtype  type;
  GdkPixbuf      *image;

  gint            item_id;

  GtkWidget      *item_image;
  GtkWidget      *item_name;
  GtkWidget      *price_label;
  GtkWidget      *selected_item;
  GtkWidget      *item_price;
  GtkWidget      *item_qt;
};

G_DEFINE_TYPE (PosmanItem, posman_item, GTK_TYPE_FLOW_BOX_CHILD)

enum {
  PROP_0,
  PROP_TYPE,
  PROP_IMAGE,
  PROP_PRICE,
  PROP_NAME,
  PROP_SELECTED,
  PROP_QT,
  PROP_ID,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

PosmanItem *
posman_item_new (const  gchar     *name,
                 const  gchar     *image,
                 const  gint       cust_id)
{
  return g_object_new (POSMAN_TYPE_ITEM,
                       "name",name,
                       "image",image,
                       "item-id",cust_id,NULL);
}

PosmanItem *
posman_item_new_category (const  gchar     *name,
                          const  gchar     *image,
                          const  gint       cust_id)
{
  return g_object_new (POSMAN_TYPE_ITEM,
                       "type",posman_type_item_category,
                       "name" ,name,
                       "image",image,
                       "item-id",cust_id,NULL);
}

PosmanItem *
posman_item_new_stock(const  gchar     *name,
                      const  gchar     *image,
                      const  gfloat     price,
                      const  gint       quantity,
                      const  gint       cust_id)
{
  return g_object_new (POSMAN_TYPE_ITEM,
                       "type",posman_type_item_stock,
                       "name" ,name,
                       "image",image,
                       "price",price,
                       "quantity",quantity,
                       "item-id",cust_id, NULL);
}

static void
posman_item_finalize (GObject *object)
{
  PosmanItem *self = (PosmanItem *)object;

  G_OBJECT_CLASS (posman_item_parent_class)->finalize (object);
}

static void
posman_item_constructed(GObject *object)
{
  PosmanItem *self = (PosmanItem *)object;

  G_OBJECT_CLASS (posman_item_parent_class)->constructed (object);

  if (self->type != posman_type_item_stock)
    {
      gtk_widget_set_visible ( self->item_price, FALSE);
      gtk_widget_set_visible ( self->item_qt, FALSE);
    }
}

static void
posman_item_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  PosmanItem *self = POSMAN_ITEM (object);

  switch (prop_id)
    {
      default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
posman_item_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  PosmanItem *self = POSMAN_ITEM (object);

  switch (prop_id)
    {
      case PROP_TYPE:
        posman_item_set_item_type(self,g_value_get_int(value));
        break;
      case PROP_IMAGE:
        posman_item_set_image (self, g_value_get_string (value));
        break;
      case PROP_PRICE:
        posman_item_set_price (self, g_value_get_float(value));
        break;
      case PROP_NAME:
        posman_item_set_name (self, g_value_get_string (value));
        break;
      case PROP_SELECTED:
        posman_item_set_selected (self, g_value_get_boolean (value));
        break;
      case PROP_QT:
        posman_item_set_quantity(self,g_value_get_int(value));
        break;
      case PROP_ID:
        posman_item_set_id(self,g_value_get_int(value));
        break;
      default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
posman_item_class_init (PosmanItemClass *klass)
{
  GObjectClass    *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = posman_item_finalize;
  object_class->constructed = posman_item_constructed;
  object_class->get_property = posman_item_get_property;
  object_class->set_property = posman_item_set_property;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/pos/manager/posman-item.ui");
  gtk_widget_class_bind_template_child (widget_class, PosmanItem, item_image);
  gtk_widget_class_bind_template_child (widget_class, PosmanItem, item_name);
  gtk_widget_class_bind_template_child (widget_class, PosmanItem, price_label);
  gtk_widget_class_bind_template_child (widget_class, PosmanItem, selected_item);
  gtk_widget_class_bind_template_child (widget_class, PosmanItem, item_price);
  gtk_widget_class_bind_template_child (widget_class, PosmanItem, item_qt);

  properties[PROP_TYPE] = g_param_spec_int ("type",
                                            "type",
                                            "item type",
                                            posman_type_item_item,
                                            posman_type_item_category,
                                            posman_type_item_item,
                                            G_PARAM_WRITABLE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_CONSTRUCT_ONLY);

  properties[PROP_IMAGE] = g_param_spec_string ("image",
                                                "image",
                                                "image Resource path",
                                                NULL,
                                                G_PARAM_WRITABLE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_CONSTRUCT);

  properties[PROP_NAME] = g_param_spec_string ("name",
                                               "name",
                                               "name of the item",
                                               NULL,
                                               G_PARAM_WRITABLE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_PRICE] = g_param_spec_float ("price",
                                               "price",
                                               "item price",
                                               0.0,
                                               G_MAXFLOAT,
                                               0.0,
                                               G_PARAM_WRITABLE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_SELECTED] = g_param_spec_boolean ("selected",
                                                    "selected",
                                                    "item is selected",
                                                    FALSE,
                                                    G_PARAM_WRITABLE | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_QT] = g_param_spec_int ("quantity",
                                          "quantity",
                                          "item quantity",
                                          0,G_MAXINT,1,
                                          G_PARAM_WRITABLE | G_PARAM_EXPLICIT_NOTIFY );

  properties[PROP_ID] = g_param_spec_int ("item-id",
                                          "item-id",
                                          "item id",
                                          -1,G_MAXINT,-1,
                                          G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_CONSTRUCT);

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
posman_item_init (PosmanItem *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

void
posman_item_set_item_type(PosmanItem *self,posmanitemtype  type)
{
  g_return_if_fail (POSMAN_IS_ITEM (self));

  self->type = type;

  g_object_notify_by_pspec (G_OBJECT (self),properties[PROP_TYPE]);
}

posmanitemtype
posman_item_get_item_type (PosmanItem    *self)
{
  g_return_val_if_fail (POSMAN_IS_ITEM (self),-1);
  return self->type;
}

void
posman_item_set_image(PosmanItem *self,const gchar  *Resource)
{
  g_return_if_fail (POSMAN_IS_ITEM (self));

  GdkPixbuf   *image;

  if(Resource)
    image = gdk_pixbuf_new_from_resource_at_scale(Resource,72,-1,TRUE,NULL);
  else
    {
      if (self->type == posman_type_item_category)
        image = gdk_pixbuf_new_from_resource_at_scale("/org/pos/manager/data/boxes.png",
                                                      72,-1,TRUE,NULL);
      else
        image = gdk_pixbuf_new_from_resource_at_scale("/org/pos/manager/data/box.png",
                                                      72,-1,TRUE,NULL);
    }

  if (image)
    gtk_image_set_from_pixbuf (GTK_IMAGE (self->item_image), image);

  g_object_notify_by_pspec (G_OBJECT (self),properties[PROP_IMAGE]);
}

void
posman_item_set_price(PosmanItem *self,const gfloat price)
{
  g_return_if_fail (POSMAN_IS_ITEM (self));
  if (price > 0)
    {
      g_autofree gchar *price_buf = NULL;
      price_buf = g_strdup_printf ("%.2f",price);
      gtk_label_set_text(GTK_LABEL (self->price_label),price_buf);
    }
  g_object_notify_by_pspec (G_OBJECT (self),properties[PROP_PRICE]);
}

void
posman_item_set_name(PosmanItem *self,const gchar *name)
{
  g_return_if_fail (POSMAN_IS_ITEM (self));

  if (name)
    gtk_label_set_text (GTK_LABEL (self->item_name), name);
  g_object_notify_by_pspec (G_OBJECT (self),properties[PROP_NAME]);
}

void
posman_item_set_selected(PosmanItem *self,const gboolean  selected)
{
  gtk_widget_set_visible (self->selected_item, selected);

  g_object_notify_by_pspec (G_OBJECT (self),properties[PROP_SELECTED]);
}

void
posman_item_set_quantity(PosmanItem *self,gint  quantity)
{
  if (quantity > 0)
    {
      g_autofree gchar *quantity_buf = NULL;
      quantity_buf = g_strdup_printf ("%d",quantity);
      gtk_label_set_text(GTK_LABEL (self->item_qt),quantity_buf);
    }

  g_object_notify_by_pspec (G_OBJECT (self),properties[PROP_QT]);
}

void
posman_item_set_id(PosmanItem *self,gint  item_id)
{
  g_return_if_fail (POSMAN_IS_ITEM (self));
  self->item_id = item_id;

  g_object_notify_by_pspec (G_OBJECT (self),properties[PROP_ID]);
}

int
posman_item_get_id(PosmanItem *self)
{
  g_return_val_if_fail (POSMAN_IS_ITEM (self),-1);
  return self->item_id;
}
