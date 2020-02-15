#include "posman-panel-row-cust.h"

struct _PosmanPanelRowCust
{
  GtkListBoxRow parent_instance;

  gint64        id;
  gchar         *name;

  GtkWidget     *cust_label_name;
  GtkWidget     *remove_button;
  GtkWidget     *action_area;
};

G_DEFINE_TYPE (PosmanPanelRowCust, posman_panel_row_cust, GTK_TYPE_LIST_BOX_ROW)

enum {
  PROP_0,
  PROP_ID,
  PROP_NAME,
  PROP_ACTION_AREA,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

PosmanPanelRowCust *
posman_panel_row_cust_new (void)
{
  return g_object_new (POSMAN_TYPE_PANEL_ROW_CUST, NULL);
}

static void
posman_panel_row_cust_finalize (GObject *object)
{
  PosmanPanelRowCust *self = (PosmanPanelRowCust *)object;

  g_free(self->name);

  G_OBJECT_CLASS (posman_panel_row_cust_parent_class)->finalize (object);
}

static void
posman_panel_row_cust_dispose(GObject *object)
{
  PosmanPanelRowCust *self = (PosmanPanelRowCust *)object;

  if (self->action_area)
    {
      gtk_widget_destroy (self->action_area);
    }

  G_OBJECT_CLASS (posman_panel_row_cust_parent_class)->dispose (object);
}

static void
posman_panel_row_cut_constructed(GObject *object)
{
  PosmanPanelRowCust *self = (PosmanPanelRowCust *)object;
  G_OBJECT_CLASS (posman_panel_row_cust_parent_class)->constructed (object);
}

static void
posman_panel_row_cust_get_property (GObject    *object,
                                    guint       prop_id,
                                    GValue     *value,
                                    GParamSpec *pspec)
{
  PosmanPanelRowCust *self = POSMAN_PANEL_ROW_CUST (object);

  switch (prop_id)
    {
      case PROP_ACTION_AREA:
        g_value_set_object (value, self->action_area);
        break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
posman_panel_row_cust_set_property (GObject      *object,
                                    guint         prop_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
  PosmanPanelRowCust *self = POSMAN_PANEL_ROW_CUST (object);

  switch (prop_id)
    {
    case PROP_NAME:
      posman_panel_row_cust_set_name(self,g_value_get_string (value));
      break;
    case PROP_ID:
      posman_panel_row_cust_set_id (self, g_value_get_int64 (value));
      break;
    case PROP_ACTION_AREA:
      posman_panel_row_cust_set_action_area (self,
                                             g_value_get_object (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
posman_panel_row_cust_class_init (PosmanPanelRowCustClass *klass)
{
  GObjectClass    *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = posman_panel_row_cust_finalize;
  object_class->dispose = posman_panel_row_cust_dispose;
  object_class->constructed = posman_panel_row_cut_constructed;
  object_class->get_property = posman_panel_row_cust_get_property;
  object_class->set_property = posman_panel_row_cust_set_property;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/pos/manager/posman-panel-row-cust.ui");

  gtk_widget_class_bind_template_child (widget_class, PosmanPanelRowCust, cust_label_name);
  gtk_widget_class_bind_template_child (widget_class, PosmanPanelRowCust, remove_button);

  properties[PROP_ID] =
  g_param_spec_int64 ("cust-id",
                      "cust-id",
                      "coustomer id",
                      0,G_MAXINT64,0,
                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_NAME] =
  g_param_spec_string ("cust-name",
                       "cust-name",
                       "coustomer name",
                       NULL,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_EXPLICIT_NOTIFY);

  properties[PROP_ACTION_AREA] =
  g_param_spec_object("action-area",
                      "action-area",
                      "action area object for binding",
                      POSMAN_TYPE_ACTION_AREA,
                      G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties(object_class ,N_PROPS ,properties );

}

static void
posman_panel_row_cust_init (PosmanPanelRowCust *self)
{
  g_type_ensure(POSMAN_TYPE_PANEL_ROW_CUST);

  gtk_widget_init_template (GTK_WIDGET (self));

}

void
posman_panel_row_cust_set_name(PosmanPanelRowCust *self,const gchar *name)
{
  g_return_if_fail (POSMAN_IS_PANEL_ROW_CUST (self));

  self->name = g_strdup (name);
  gtk_label_set_text (GTK_LABEL(self->cust_label_name), self->name);
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_NAME]);
}

void
posman_panel_row_cust_set_id(PosmanPanelRowCust *self,gint64 id)
{
  g_return_if_fail (POSMAN_IS_PANEL_ROW_CUST (self));

  self->id = id;
  if (self->id)
  gtk_actionable_set_action_target(GTK_ACTIONABLE (self->remove_button),
                                   "x",self->id);
  gtk_actionable_set_action_target(GTK_ACTIONABLE (self),
                                   "x",self->id);
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_ID]);
}

gint64
posman_panel_row_cust_get_id(PosmanPanelRowCust *self)
{
  g_return_val_if_fail (POSMAN_IS_PANEL_ROW_CUST (self),-1);

  return self->id;
}

void
posman_panel_row_cust_set_action_area(PosmanPanelRowCust *self,
                                           PosmanActionArea   *action_area)
{
  g_return_if_fail (POSMAN_IS_PANEL_ROW_CUST (self));
  g_return_if_fail (action_area == NULL || POSMAN_IS_ACTION_AREA (action_area));

  if (self->action_area)
    g_object_unref(self->action_area);

  self->action_area = g_object_ref (action_area);

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_ACTION_AREA]);
}

PosmanActionArea *
posman_panel_row_cust_get_action_area(PosmanPanelRowCust *self)
{
  g_return_val_if_fail (POSMAN_IS_PANEL_ROW_CUST (self),NULL);

  if(self->action_area)
  return (PosmanActionArea *)self->action_area;
  return NULL;
}
