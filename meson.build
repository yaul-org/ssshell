project(
  'ssshell',
  'c',
  version: '0.0.1',
  default_options: ['warning_level=3']
)

project_source_files = [
  'ssshell.c',
]

libssusb_proj = subproject('libssusb')
libssusb_dep = libssusb_proj.get_variable('libssusb_dep')

project_dependencies = [
  libssusb_dep,
]

build_args = [
]

link_args = [
]

# Target

build_args += [
  '-DPROJECT_NAME=' + meson.project_name(),
  '-DPROJECT_VERSION=' + meson.project_version(),
]

project_target = executable(
  meson.project_name(),
  project_source_files,
  dependencies: project_dependencies,
  install: true,
  c_args: build_args,
)
