# Copyright (c) 2012-2022
# See LICENSE for details.
#
# Simon Ser (emersion)
# Israel Jacquez <mrkotfw@gmail.com>

project(
  'ssshell',
  'c',
  version: '0.0.1',
  default_options: ['warning_level=3']
)

cc = meson.get_compiler('c')

project_source_files = [
  'ssshell.c',
  'shell/shell.c',
  'shell/lexer.c',
  'shell/parser.c',
  'env.c',
  'object.c',

  'commands.c',
  'commands/clear.c',
  'commands/dseld.c',
  'commands/help.c',
  'commands/quit.c',
]

libssusb_proj = subproject('libssusb')
libssusb_dep = libssusb_proj.get_variable('libssusb_dep')

if get_option('readline-provider') == 'readline'
  libreadline_dep = cc.find_library('readline', required: true)
  if libreadline_dep.found()
    add_project_arguments('-DHAVE_READLINE', language: 'c')
  endif

  if cc.has_function('rl_replace_line', prefix: '#include <stdio.h>\n#include <readline/readline.h>', dependencies: [libreadline_dep])
    add_project_arguments('-DHAVE_READLINE_REPLACE_LINE', language: 'c')
  endif
else # editline
  libreadline_dep = dependency('libedit', required: true)
  if libreadline_dep.found()
    add_project_arguments('-DHAVE_EDITLINE', language: 'c')
  endif
endif

project_dependencies = [
  libssusb_dep,
  libreadline_dep,
]

build_args = [
]

link_args = [
]

include_directories = [
  include_directories('shell'),
]

# Target

if host_machine.system() == 'windows'
  project_source_files += 'shell/conio.c'
endif

build_args += [
  '-DPROJECT_NAME="' + meson.project_name() + '"',
  '-DPROJECT_VERSION=' + meson.project_version(),
]

project_target = executable(
  meson.project_name(),
  project_source_files,
  dependencies: project_dependencies,
  install: true,
  c_args: build_args,
  include_directories: include_directories
)
