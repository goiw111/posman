#include "posman-panel-loader.h"

G_DEFINE_INTERFACE (PosmanPanelLoader, posman_panel_loader, G_TYPE_OBJECT)

static void
posman_panel_loader_default_init (PosmanPanelLoaderInterface *iface)
{

}

gboolean posman_panel_loader_get_cust(PosmanPanelLoader *self, sqlite3 *db)
{
  PosmanPanelLoaderInterface *iface = POSMAN_PANEL_LOADER_GET_IFACE (self);

  g_return_val_if_fail (POSMAN_IS_PANEL_LOADER (self), FALSE);

  if(!iface->get_cust)
    {
      g_warning ("Object of type \"%s\" does not implement required interface"
                 " method \"posman_panel_loader_get_cust\",",
                 G_OBJECT_TYPE_NAME (self));

      return FALSE;
    }
  else
    {
      return iface->get_cust(self,db);
    }
}

gboolean posman_panel_loader_get_cmnd(PosmanPanelLoader *self,
                                      sqlite3 *db,
                                      gchar *cust_id)
{
  PosmanPanelLoaderInterface *iface = POSMAN_PANEL_LOADER_GET_IFACE (self);

  g_return_val_if_fail (POSMAN_IS_PANEL_LOADER (self), FALSE);

  if (!iface->get_cmnd)
    {
      g_warning ("Object of type \"%s\" does not implement required interface"
                 " method \"posman_panel_loader_get_cmnd\",",
                 G_OBJECT_TYPE_NAME (self));
      return FALSE;
    }
  else
    {
      return iface->get_cmnd(self,db,cust_id);
    }
}
