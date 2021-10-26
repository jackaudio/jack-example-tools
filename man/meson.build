if os != 'windows'
  meson.add_postconf_script(
    '../scripts/meson_create_man_pages',
    conf_data.get('version'),
    os
  )
  meson.add_install_script(
    '../scripts/meson_install_man_pages',
    get_option('mandir')
  )
endif
