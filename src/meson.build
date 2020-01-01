posman_sources = [
  'main.c',
  'posman-window.c',
  'posman-panel-list.c',
  'posman-action-menu-cust.c',
  'posman-action-menu-cmnd.c',
  'posman-panel-row-cust.c',
  'posman-action-area.c',
]

posman_deps = [
  dependency('gio-2.0', version: '>= 2.50'),
  dependency('glib-2.0', version: '>= 2.58'),
  dependency('gtk+-3.0', version: '>= 3.24'),
  dependency('sqlite3', version: '>= 3.27.2'),
  dependency('gdk-pixbuf-2.0', version: '>= 2.38.1'),
]

gnome = import('gnome')

posman_sources += gnome.compile_resources('posman-resources',
  'posman.gresource.xml',
  c_name: 'posman'
)

executable('posman', posman_sources,
  dependencies: posman_deps,
  install: true,
)
