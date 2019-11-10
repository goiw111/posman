#include "posman-panel-loader.h"

G_DEFINE_INTERFACE (PosmanPanelLoader, posman_panel_loader, G_TYPE_OBJECT)

static void
posman_panel_loader_default_init (PosmanPanelLoaderInterface *iface)
{
    g_object_interface_install_property (iface,
                                         g_param_spec_boolean ("db-init",
                                                               "database init",
                                                               "open database file",
                                                               TRUE,
                                                               G_PARAM_READWRITE));
}

gboolean posman_panel_loader_get_cust(PosmanPanelLoader *self)
{
  PosmanPanelLoaderInterface *iface = POSMAN_PANEL_LOADER_GET_IFACE (self);

  g_return_val_if_fail (POSMAN_IS_PANEL_LOADER (self), FALSE);

  if(!iface->get_cust)
    {
      g_warning ("Object of type \"%s\" does not implement required interface"
                 " method \"set_active_panel_from_id\",",
                 G_OBJECT_TYPE_NAME (self));

      return FALSE;
    }
  else
    {
      return iface->get_cust(self);
    }
}
