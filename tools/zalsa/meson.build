
zalsa_version = '0.8.4'

zalsa_in_c_args = c_args_common + ['-DAPPNAME="zalsa_in"', '-DVERSION="@0@"'.format(zalsa_version)]
lib_zalsa_in = library(
  'zalsa_in',
  cpp_args: zalsa_in_c_args,
  name_prefix: '',
  sources: ['zita-a2j.cc', 'alsathread.cc', 'jackclient.cc', 'pxthread.cc', 'lfqueue.cc'],
  dependencies: [dep_alsa, dep_jack, dep_threads, lib_jackserver, lib_m, lib_rt, lib_zita_alsa_pcmi, lib_zita_resampler],
  install: true,
  install_dir: get_option('libdir') / 'jack',
)

zalsa_out_c_args = c_args_common + ['-DAPPNAME="zalsa_out"', '-DVERSION="@0@"'.format(zalsa_version)]
lib_zalsa_out = library(
  'zalsa_out',
  cpp_args: zalsa_out_c_args,
  name_prefix: '',
  sources: ['zita-j2a.cc', 'alsathread.cc', 'jackclient.cc', 'pxthread.cc', 'lfqueue.cc'],
  dependencies: [dep_alsa, dep_jack, dep_threads, lib_jackserver, lib_m, lib_rt, lib_zita_alsa_pcmi, lib_zita_resampler],
  install: true,
  install_dir: get_option('libdir') / 'jack',
)
