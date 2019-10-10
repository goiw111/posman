
#include "posman-config.h"
#include "posman-window.h"

struct _PosmanWindow
{
  GtkApplicationWindow  parent_instance;

  /* Template widgets */
  GtkHeaderBar        *header_bar;
  GtkLabel            *label;
};

G_DEFINE_TYPE (PosmanWindow, posman_window, GTK_TYPE_APPLICATION_WINDOW)

static void
posman_window_class_init (PosmanWindowClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/pos/manager/posman-window.ui");
 /* gtk_widget_class_bind_template_child (widget_class, PosmanWindow, header_bar);*/
}

static void
posman_window_init (PosmanWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}
