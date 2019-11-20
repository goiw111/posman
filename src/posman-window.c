
#include "posman-config.h"
#include "posman-window.h"
#include "posman-panel-list.h"
#include <sqlite3.h>

struct _PosmanWindow
{
  GtkApplicationWindow  parent_instance;

  /* Template widgets */
  GtkWidget             *header_bar;
  GtkWidget             *previous_button;
  GtkWidget             *panel_list;

  /* database */
  sqlite3               *db;


};

G_DEFINE_TYPE (PosmanWindow, posman_window, GTK_TYPE_APPLICATION_WINDOW)

static int
posman_window_init_database(PosmanWindow *self)
{
  g_autofree char *dir = NULL;

  dir = g_strdup_printf("%s/""posman.db",g_get_user_data_dir());

  int rc = sqlite3_open(dir, &(self->db));

  if (rc != SQLITE_OK)
    {
      g_warning ("Cannot open database: %s\n", sqlite3_errmsg(self->db));
      sqlite3_close (self->db);
    }
}

/* callback */

void panel_list_view_changed_cb (PosmanPanelList *panel_list,
                                 GParamSpec  *pspec,
                                 PosmanWindow    *self)
{
  gboolean is_main_view;

  is_main_view = posman_panel_list_get_view (panel_list) == posman_panel_list_main;

  gtk_widget_set_visible (self->previous_button, !is_main_view);
}

static void
previous_button_clicked_cb(GtkButton *button,
                           PosmanWindow    *self)
{
  posman_panel_list_set_view (POSMAN_PANEL_LIST (self->panel_list),
                              posman_panel_list_main);
}

/* object vfonc */

static void posman_window_finalize(GObject *object)
{
  PosmanWindow *self = POSMAN_WINDOW (object);
  sqlite3_close(self->db);

  G_OBJECT_CLASS (posman_window_parent_class)->finalize (object);
}

static void
posman_window_class_init (PosmanWindowClass *klass)
{
  GtkWidgetClass  *widget_class = GTK_WIDGET_CLASS (klass);
  GObjectClass    *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = posman_window_finalize;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/pos/manager/posman-window.ui");

  gtk_widget_class_bind_template_child (widget_class, PosmanWindow, previous_button);
  gtk_widget_class_bind_template_child (widget_class, PosmanWindow, panel_list);
  gtk_widget_class_bind_template_callback (widget_class, panel_list_view_changed_cb);
  gtk_widget_class_bind_template_callback (widget_class, previous_button_clicked_cb);

  g_type_ensure(POSMAN_TYPE_PANEL_LIST);
}

static void
posman_window_init (PosmanWindow *self)
{
  GtkWidget *panel;
  gtk_widget_init_template (GTK_WIDGET (self));

  posman_window_init_database(self);

  posman_panel_list_main_model_init(POSMAN_PANEL_LIST (self->panel_list),self->db);
  posman_panel_list_main_load_panels(POSMAN_PANEL_LIST (self->panel_list));

}


