
#include "posman-config.h"
#include "posman-window.h"
#include "posman-panel-list.h"

struct _PosmanWindow
{
  GtkApplicationWindow  parent_instance;

  /* Template widgets */
  GtkWidget             *header_bar;
  GtkWidget             *previous_button;
  GtkWidget             *panel_list;
};

G_DEFINE_TYPE (PosmanWindow, posman_window, GTK_TYPE_APPLICATION_WINDOW)

void panel_list_view_changed_cb (PosmanPanelList *panel_list,
                            GParamSpec  *pspec,
                            PosmanWindow    *self)
{
  gboolean is_main_view;

  is_main_view = posman_panel_list_get_view (panel_list) == posman_panel_list_main;

  gtk_widget_set_visible (self->previous_button, !is_main_view);
}

static void
posman_window_class_init (PosmanWindowClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/pos/manager/posman-window.ui");

  gtk_widget_class_bind_template_child (widget_class, PosmanWindow, previous_button);
  gtk_widget_class_bind_template_child (widget_class, PosmanWindow, panel_list);
  gtk_widget_class_bind_template_callback (widget_class, panel_list_view_changed_cb);

  g_type_ensure(POSMAN_TYPE_PANEL_LIST);
}

static void
posman_window_init (PosmanWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

}
