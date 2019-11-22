posman_sources = [
  'main.c',
  'posman-window.c',
  'posman-panel-list.c',
  'posman-panel-model.c',
  'posman-panel-loader.c',
  'posman-action-menu.c',
]

sqlite_required = '3.27.2'

posman_deps = [
  dependency('gio-2.0', version: '>= 2.50'),
  dependency('glib-2.0', version: '>= 2.58'),
  dependency('gtk+-3.0', version: '>= 3.24'),
  dependency('sqlite3', version: '>= 3.27.2'),
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
