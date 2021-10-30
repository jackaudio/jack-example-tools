project(
  'jack-example-tools',
  ['c', 'cpp'],
  meson_version: '>=0.50.0',
  license: ['GPL2+'],
  version: '0.1.0',
)

os = build_machine.system()
cc = meson.get_compiler('c')
lib_m = cc.find_library('m')
lib_rt = cc.find_library('rt')

dep_alsa = dependency('alsa', version: '>=1.0.18', required: false)
dep_celt = dependency('celt', version: '>=0.11.0')
dep_jack = dependency('jack')
jack_version = dep_jack.version()
lib_jackserver = cc.find_library('jackserver', required: true)
lib_jacknet = cc.find_library('jacknet', required: false)
dep_opus = dependency('opus', version: '>=0.9.0')
dep_readline = dependency('readline')
dep_samplerate = dependency('samplerate')
dep_sndfile = dependency('sndfile')
dep_threads = dependency('threads')
lib_zita_alsa_pcmi = cc.find_library('zita-alsa-pcmi', required: true)
lib_zita_resampler = cc.find_library('zita-resampler', required: true)

conf_data = configuration_data()
conf_data.set('version', meson.project_version())

c_args_common = [
  '-D__PROJECT_VERSION__="@0@"'.format(conf_data.get('version')),
]

if jack_version.version_compare('>=0.109.0') and jack_version.version_compare('<0.200.0')
  message('Detected jack1 compatible headers')
  c_args_common += ['-D__JACK1__']
endif
if (jack_version.version_compare('>=0.58') and jack_version.version_compare('<=0.71')) or jack_version.version_compare('>=1.9.0')
  message('Detected jack2 compatible headers')
  c_args_common += ['-D__JACK2__']
endif

subdir('tools')
subdir('example-clients')
subdir('man')
