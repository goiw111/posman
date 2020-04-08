#include "posman-action-controler.h"

struct _PosmanActionControler
{
  GtkListBoxRow parent_instance;

  GtkWidget    *name;
  gint          cmnd_id;
};

G_DEFINE_TYPE (PosmanActionControler, posman_action_controler, GTK_TYPE_LIST_BOX_ROW)

enum {
  PROP_0,
  PROP_CMND_ID,
  N_PROPS
};


enum {
  CLOSE,

  LAST_SIGNAL
};

static GParamSpec *properties [N_PROPS];
static guint       signals[LAST_SIGNAL];

static void
button_close_cb(GtkButton *button,
                gpointer   user_data)
{
  PosmanActionControler *self = (PosmanActionControler *)user_data;

  gtk_widget_destroy (GTK_WIDGET(self));
}

PosmanActionControler *
posman_action_controler_new (const gchar    *name,gint  cmnd_id)
{
  PosmanActionControler *controler = g_object_new (POSMAN_TYPE_ACTION_CONTROLER,
                                                   "cmnd-id",cmnd_id,
                                                   NULL);
  gtk_label_set_text(GTK_LABEL(controler->name),name);
  return controler;
}

static void
posman_action_controler_finalize (GObject *object)
{
  PosmanActionControler *self = (PosmanActionControler *)object;

  G_OBJECT_CLASS (posman_action_controler_parent_class)->finalize (object);
}

static void
posman_action_controler_get_property (GObject    *object,
                                      guint       prop_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
  PosmanActionControler *self = POSMAN_ACTION_CONTROLER (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
posman_action_controler_set_property (GObject      *object,
                                      guint         prop_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
  PosmanActionControler *self = POSMAN_ACTION_CONTROLER (object);

  switch (prop_id)
    {
      case PROP_CMND_ID:
        posman_action_controler_set_cmnd_id(self,g_value_get_int (value));
        break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
posman_action_controler_class_init (PosmanActionControlerClass *klass)
{
  GObjectClass    *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = posman_action_controler_finalize;
  object_class->get_property = posman_action_controler_get_property;
  object_class->set_property = posman_action_controler_set_property;

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/pos/manager/posman-action-controler.ui");

  gtk_widget_class_bind_template_child (widget_class, PosmanActionControler,
                                        name);
  gtk_widget_class_bind_template_callback (widget_class, button_close_cb);

  properties[PROP_CMND_ID] = g_param_spec_int ("cmnd-id",
                                               "cmnd-id",
                                               "the cmnd id",
                                               -1,G_MAXINT,-1,
                                               G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_CONSTRUCT_ONLY);

   g_object_class_install_properties (object_class, N_PROPS, properties);

  signals[CLOSE] = g_signal_new("close",
                                G_OBJECT_CLASS_TYPE (object_class),
                                G_SIGNAL_RUN_FIRST,
                                0,NULL,NULL,NULL,
                                G_TYPE_NONE,0);
}

static void
posman_action_controler_init (PosmanActionControler *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

const gchar *
posman_action_controler_get_name(PosmanActionControler *self)
{
  g_return_val_if_fail (POSMAN_IS_ACTION_CONTROLER (self),NULL);

  return gtk_label_get_text (GTK_LABEL(self->name));
}

void
posman_action_controler_set_cmnd_id(PosmanActionControler *self,gint    cmnd_id)
{
  g_return_if_fail (POSMAN_IS_ACTION_CONTROLER (self));

  self->cmnd_id = cmnd_id;

  g_object_notify_by_pspec (G_OBJECT (self),properties[PROP_CMND_ID]);
}

gint
posman_action_controler_get_cmnd_id(PosmanActionControler *self)
{
  g_return_val_if_fail (POSMAN_IS_ACTION_CONTROLER (self),-1);

  return self->cmnd_id;
}
