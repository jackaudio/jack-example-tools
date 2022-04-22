exe_jack_control_client = executable(
  'jack_control_client',
  sources: ['control.c'],
  dependencies: [dep_jack],
  install: true
)

exe_jack_cpu_load = executable(
  'jack_cpu_load',
  sources: ['cpu_load.c'],
  dependencies: [dep_jack],
  install: true
)

if build_jack_rec
  exe_jack_rec = executable(
    'jack_rec',
    sources: ['capture_client.c'],
    dependencies: [dep_jack, dep_sndfile, dep_threads],
    install: true
  )
endif

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

lib_jack_internal_metro = library(
  'jack_internal_metro',
  name_prefix: '',
  sources: ['internal_metro.cpp'],
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

exe_jack_midi_latency_test = executable(
  'jack_midi_latency_test',
  sources: ['midi_latency_test.c'],
  dependencies: [dep_jack, dep_threads, lib_m],
  install: true
)

if build_jack_net
  exe_jack_net_master = executable(
    'jack_net_master',
    sources: ['netmaster.c'],
    dependencies: [lib_jacknet],
    install: true
  )

  exe_jack_net_slave = executable(
    'jack_net_slave',
    sources: ['netslave.c'],
    dependencies: [lib_jacknet],
    install: true
  )
endif

if host_machine.system() != 'windows'
  if has_jackctl_server_create2
    c_args_jack_server_control = c_args_common + ['-D__JACK2__']
  else
    c_args_jack_server_control = c_args_common + ['-D__JACK1__']
  endif
  exe_jack_server_control = executable(
    'jack_server_control',
    c_args: c_args_jack_server_control,
    sources: ['server_control.c'],
    dependencies: [dep_jack, lib_jackserver],
    install: true
  )
endif

exe_jack_showtime = executable(
  'jack_showtime',
  sources: ['showtime.c'],
  dependencies: [dep_jack],
  install: true
)

exe_jack_simdtests = executable(
  'jack_simdtests',
  sources: ['simdtests.cpp'],
  include_directories: ['../common'],
  dependencies: [lib_m],
  install: true
)

exe_jack_simple_client = executable(
  'jack_simple_client',
  sources: ['simple_client.c'],
  dependencies: [dep_jack, lib_m],
  install: true
)

# dont install, jack session is deprecated
exe_jack_simple_session_client = executable(
  'jack_simple_session_client',
  sources: ['simple_session_client.c'],
  dependencies: [dep_jack],
  c_args: ['-Wno-deprecated-declarations'],
  install: false
)

exe_jack_transport_client = executable(
  'jack_transport_client',
  sources: ['transport_client.c'],
  dependencies: [dep_jack],
  install: true
)

exe_jack_thru_client = executable(
  'jack_thru_client',
  sources: ['thru_client.c'],
  dependencies: [dep_jack],
  install: true
)

exe_jack_zombie = executable(
  'jack_zombie',
  sources: ['zombie.c'],
  dependencies: [dep_jack],
  install: true
)
