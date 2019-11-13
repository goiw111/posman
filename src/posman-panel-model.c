#include "posman-panel-model.h"
#include "posman-panel-loader.h"
#include <gtk/gtk.h>
#include <sqlite3.h>

struct _PosmanPanelModel
{
  GtkListStore            parent_instance;

  /*model type */
  posmanpanelmodeltype    type;

};

static void
posman_panel_loader_init (PosmanPanelLoaderInterface *iface);

G_DEFINE_TYPE_WITH_CODE (PosmanPanelModel,
                         posman_panel_model,
                         GTK_TYPE_LIST_STORE,
                         G_IMPLEMENT_INTERFACE (POSMAN_TYPE_PANEL_LOADER,
                                                posman_panel_loader_init))

enum {
  PROP_0,
  PROP_TYPE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];


PosmanPanelModel *
posman_panel_model_new (void)
{
  return g_object_new (POSMAN_TYPE_PANEL_MODEL, NULL);
}

static gboolean posman_panel_model_get_cust(PosmanPanelLoader *self, sqlite3  *db)
{
  PosmanPanelModel *model = POSMAN_PANEL_MODEL (self);
  sqlite3_stmt *stmt;
  int          rc;

  rc = sqlite3_prepare_v2(db,"SELECT id, full_name FROM customer;",-1,
                          &stmt,
                          NULL);

  if (rc != SQLITE_OK)
    {
      g_warning ("Failed to execute statement: %s\n", sqlite3_errmsg(db));
      return FALSE;
    }

  while(sqlite3_step(stmt) != SQLITE_DONE)
    posman_panel_model_add_cust (model,
                                 sqlite3_column_text (stmt, COL_ID),
                                 sqlite3_column_text (stmt, COL_NAME));

  sqlite3_finalize(stmt);
  return TRUE;

}

void posman_panel_loader_init (PosmanPanelLoaderInterface *iface)
{
  iface->get_cust = posman_panel_model_get_cust;
}

static void
posman_panel_model_finalize (GObject *object)
{
  /* PosmanPanelModel *self = (PosmanPanelModel *)object; */

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
    case PROP_TYPE:
      g_value_set_int (value, self->type);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
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
    case PROP_TYPE:
      self->type = g_value_get_int (value);
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

  properties[PROP_TYPE] = g_param_spec_int("type",
                                           "type",
                                           "type of the model main or cust",
                                           posmanpanelmodelmain,
                                           posmanpanelmodelcust,
                                           posmanpanelmodelmain,
                                           G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  g_object_class_install_properties (object_class, N_PROPS, properties);

}

static void
posman_panel_model_init (PosmanPanelModel *self)
{

}

void
posman_panel_model_main_init(PosmanPanelModel *self)
{
  g_return_if_fail (self->type == posmanpanelmodelmain);

  GType types[] = {G_TYPE_STRING,G_TYPE_STRING};
  gtk_list_store_set_column_types(GTK_LIST_STORE (self),N_COL,types);
}

void
posman_panel_model_add_cust(PosmanPanelModel *self, const unsigned char *id, const unsigned char *name)
{
  g_return_if_fail (self->type == posmanpanelmodelmain);

  gtk_list_store_insert_with_values(GTK_LIST_STORE (self),NULL,0,
                                    COL_ID,
                                    id,
                                    COL_NAME,
                                    name,
                                    -1);

}



