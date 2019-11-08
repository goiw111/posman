#include "posman-panel-model.h"
#include "posman-panel-loader.h"
#include <gtk/gtk.h>
#include <sqlite3.h>

struct _PosmanPanelModel
{
  GtkListStore parent_instance;


  /* database */
  sqlite3 *db;
  int rc;
};

static void posman_panel_loader_init (PosmanPanelLoaderInterface *iface);

G_DEFINE_TYPE_WITH_CODE (PosmanPanelModel,
                         posman_panel_model,
                         GTK_TYPE_TREE_MODEL,
                         G_IMPLEMENT_INTERFACE (POSMAN_TYPE_PANEL_LOADER,
                                                posman_panel_loader_init))

enum {
  PROP_0,
  PROP_DB_INIT,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

void posman_panel_loader_init (PosmanPanelLoaderInterface *iface)
{

}

PosmanPanelModel *
posman_panel_model_new (void)
{
  return g_object_new (POSMAN_TYPE_PANEL_MODEL, NULL);
}

static void
posman_panel_model_finalize (GObject *object)
{
  PosmanPanelModel *self = (PosmanPanelModel *)object;

  G_OBJECT_CLASS (posman_panel_model_parent_class)->finalize (object);
}

static void
posman_panel_model_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  PosmanPanelModel *self = POSMAN_PANEL_MODEL (object);

  switch (prop_id)
    {
    case PROP_DB_INIT:
      if(self->rc != SQLITE_OK) g_value_set_boolean (value, FALSE);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

gboolean bd_init (PosmanPanelModel *self ,const GValue *value )
{
  if(g_value_get_boolean(value) && (self->rc != SQLITE_OK)){
    self->rc = sqlite3_open("test.db", &(self->db));
    if (self->rc != SQLITE_OK) {

        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(self->db));
        sqlite3_close(self->db);
    }
  }
  if(g_value_get_boolean(value) && (self->rc == SQLITE_OK)){
    sqlite3_close(self->db);
  }

}

static void
posman_panel_model_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  PosmanPanelModel *self = POSMAN_PANEL_MODEL (object);

  switch (prop_id)
    {
    case PROP_DB_INIT:
      bd_init(self,value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
posman_panel_model_class_init (PosmanPanelModelClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);


  object_class->finalize = posman_panel_model_finalize;
  object_class->get_property = posman_panel_model_get_property;
  object_class->set_property = posman_panel_model_set_property;

  g_object_class_override_property (object_class, PROP_DB_INIT, "db-init");

}

static void
posman_panel_model_init (PosmanPanelModel *self)
{
  GType types[] = {G_TYPE_INT,G_TYPE_STRING,G_TYPE_INT};
  gtk_list_store_set_column_types(GTK_LIST_STORE (self),N_COL,types);
}
