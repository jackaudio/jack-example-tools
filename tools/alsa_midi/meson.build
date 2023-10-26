
lib_alsa_midi = library(
  'alsa_midi',
  c_args: c_args_common + ['-DALSA_MIDI_INTERNAL_CLIENT'],
  name_prefix: '',
  sources: ['alsa_midi.c', 'port.c', 'port_hash.c', 'port_thread.c', 'list.c'],
  dependencies: [dep_alsa, dep_jack, dep_threads, lib_jackserver, lib_m, lib_rt],
  install: true,
  install_dir: get_option('libdir') / 'jack',
)
