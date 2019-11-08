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
