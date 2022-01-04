man_pages = [
  'jack_bufsize',
  'jack_connect',
  'jack_disconnect',
  'jack_freewheel',
  'jack_impulse_grabber',
  'jack_iodelay',
  'jack_load',
  'jack_lsp',
  'jack_metro',
  'jack_monitor_client',
  'jack_property',
  'jack_samplerate',
  'jack_showtime',
  'jack_simple_client',
  'jack_transport',
  'jack_unload',
  'jack_wait',
]

if build_alsa_in_out
  man_pages += ['alsa_in', 'alsa_out']
endif
if build_jack_netsource
  man_pages += ['jack_netsource']
endif
if build_jack_rec
  man_pages += ['jack_rec']
endif

if os != 'windows'
  foreach man_page: man_pages
    configure_file(
      configuration: conf_data,
      input: man_page + '.0',
      output: man_page + '.1',
      install_dir: get_option('mandir') + '/man1',
      install: true,
    )
  endforeach
endif
