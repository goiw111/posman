posman_sources = [
  'main.c',
  'posman-window.c',
  'posman-panel-list.c',
]

posman_deps = [
  dependency('gio-2.0', version: '>= 2.50'),
  dependency('gtk+-3.0', version: '>= 3.22'),
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
