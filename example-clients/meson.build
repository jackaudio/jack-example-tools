exe_jack_rec = executable(
  'jack_rec',
  sources: ['capture_client.c'],
  dependencies: [dep_jack, dep_sndfile, dep_threads],
  install: true
)

exe_jack_impulse_grabber = executable(
  'jack_impulse_grabber',
  sources: ['impulse_grabber.c'],
  dependencies: [dep_jack],
  install: true
)

lib_jack_inprocess = library(
  'jack_inprocess',
  name_prefix: '',
  sources: ['inprocess.c'],
  dependencies: [dep_jack],
  install: true,
  install_dir: get_option('libdir') / 'jack',
)

lib_jack_intime = library(
  'jack_intime',
  name_prefix: '',
  sources: ['intime.c'],
  dependencies: [dep_jack],
  install: true,
  install_dir: get_option('libdir') / 'jack',
)

exe_jack_latent_client = executable(
  'jack_latent_client',
  sources: ['latent_client.c'],
  dependencies: [dep_jack],
  install: true
)

exe_jack_metro = executable(
  'jack_metro',
  sources: ['metro.c'],
  dependencies: [dep_jack, lib_m],
  install: true
)

exe_jack_midiseq = executable(
  'jack_midiseq',
  sources: ['midiseq.c'],
  dependencies: [dep_jack],
  install: true
)

exe_jack_midisine = executable(
  'jack_midisine',
  sources: ['midisine.c'],
  dependencies: [dep_jack, lib_m],
  install: true
)

exe_jack_server_control = executable(
  'jack_server_control',
  c_args: c_args_common,
  sources: ['server_control.c'],
  dependencies: [dep_jack, lib_jackserver],
  install: true
)

exe_jack_showtime = executable(
  'jack_showtime',
  sources: ['showtime.c'],
  dependencies: [dep_jack],
  install: true
)

exe_jack_simple_client = executable(
  'jack_simple_client',
  sources: ['simple_client.c'],
  dependencies: [dep_jack],
  install: true
)

exe_jack_simple_session_client = executable(
  'jack_simple_session_client',
  sources: ['simple_session_client.c'],
  dependencies: [dep_jack],
  install: true
)

exe_jack_transport_client = executable(
  'jack_transport_client',
  sources: ['transport_client.c'],
  dependencies: [dep_jack],
  install: true
)
