exe_jack_alias = executable(
  'jack_alias',
  c_args: c_args_common,
  sources: ['alias.c'],
  dependencies: [dep_jack],
  install: true
)

exe_alsa_in = executable(
  'alsa_in',
  sources: ['alsa_in.c', '../common/memops.c'],
  include_directories: ['../common'],
  dependencies: [dep_alsa, dep_jack, dep_samplerate, lib_m],
  install: true
)
exe_alsa_out = executable(
  'alsa_out',
  sources: ['alsa_out.c', '../common/memops.c'],
  include_directories: ['../common'],
  dependencies: [dep_alsa, dep_jack, dep_samplerate, lib_m],
  install: true
)

exe_jack_bufsize = executable(
  'jack_bufsize',
  sources: ['bufsize.c'],
  dependencies: [dep_jack, dep_threads],
  install: true
)

exe_jack_connect = executable(
  'jack_connect',
  c_args: c_args_common,
  sources: ['connect.c'],
  dependencies: [dep_jack],
  install: true
)
if os != 'windows'
meson.add_install_script(
  '../scripts/meson_create_symlink',
  get_option('bindir'),
  'jack_connect',
  'jack_disconnect'
)
endif

exe_jack_evmon = executable(
  'jack_evmon',
  sources: ['evmon.c'],
  dependencies: [dep_jack],
  install: true
)

exe_jack_freewheel = executable(
  'jack_freewheel',
  sources: ['freewheel.c'],
  dependencies: [dep_jack],
  install: true
)

exe_jack_iodelay = executable(
  'jack_iodelay',
  sources: ['iodelay.c'],
  dependencies: [dep_jack, lib_m],
  install: true
)

exe_jack_load = executable(
  'jack_load',
  sources: ['ipload.c'],
  dependencies: [dep_jack],
  install: true
)

exe_jack_unload = executable(
  'jack_unload',
  sources: ['ipunload.c'],
  dependencies: [dep_jack],
  install: true
)

exe_jack_unload = executable(
  'jack_load_test',
  c_args: c_args_common,
  sources: ['load_test.c'],
  dependencies: [dep_jack],
  install: true
)

exe_jack_lsp = executable(
  'jack_lsp',
  c_args: c_args_common,
  sources: ['lsp.c'],
  dependencies: [dep_jack],
  install: true
)

exe_jack_midi_dump = executable(
  'jack_midi_dump',
  sources: ['midi_dump.c'],
  dependencies: [dep_jack, dep_threads],
  install: true
)

exe_jack_monitor_client = executable(
  'jack_monitor_client',
  sources: ['monitor_client.c'],
  dependencies: [dep_jack],
  install: true
)

exe_jack_netsource = executable(
  'jack_netsource',
  c_args: ['-DNO_JACK_ERROR'],
  sources: ['netsource.c', '../common/netjack_packet.c'],
  include_directories: ['../common'],
  dependencies: [dep_celt, dep_opus, dep_jack, dep_samplerate, lib_m],
  install: true
)

exe_jack_property = executable(
  'jack_property',
  sources: ['property.c'],
  dependencies: [dep_jack],
  install: true
)

exe_jack_samplerate = executable(
  'jack_samplerate',
  sources: ['samplerate.c'],
  dependencies: [dep_jack],
  install: true
)

exe_jack_session_notify = executable(
  'jack_session_notify',
  sources: ['session_notify.c'],
  dependencies: [dep_jack],
  install: true
)

exe_jack_transport = executable(
  'jack_transport',
  sources: ['transport.c'],
  dependencies: [dep_jack, dep_readline],
  install: true
)

exe_jack_tw = executable(
  'jack_tw',
  sources: ['tw.c'],
  dependencies: [dep_jack],
  install: true
)

exe_jack_wait = executable(
  'jack_wait',
  c_args: c_args_common,
  sources: ['wait.c'],
  dependencies: [dep_jack],
  install: true
)

subdir('zalsa')
