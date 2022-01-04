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

alsa_required = false
if get_option('alsa_in_out').enabled() or get_option('zalsa').enabled()
  alsa_required = true
endif

libsamplerate_required = false
if get_option('alsa_in_out').enabled() or get_option('jack_netsource').enabled()
  libsamplerate_required = true
endif

dep_alsa = dependency('alsa', version: '>=1.0.18', required: alsa_required)
dep_jack = dependency('jack')
jack_version = dep_jack.version()
lib_jackserver = cc.find_library('jackserver', required: true)
lib_jacknet = cc.find_library('jacknet', required: get_option('jack_net'))
dep_opus = dependency('opus', version: '>=0.9.0', required: get_option('opus_support'))
dep_readline = dependency('readline')
dep_samplerate = dependency('samplerate', required: libsamplerate_required)
dep_sndfile = dependency('sndfile', required: get_option('jack_rec'))
dep_threads = dependency('threads')
lib_zita_alsa_pcmi = cc.find_library('zita-alsa-pcmi', required: get_option('zalsa'))
lib_zita_resampler = cc.find_library('zita-resampler', required: get_option('zalsa'))

build_alsa_in_out = false
if get_option('alsa_in_out').enabled() or (
  get_option('alsa_in_out').auto() and dep_alsa.found() and dep_samplerate.found()
)
  build_alsa_in_out = true
endif

build_jack_net = false
if get_option('jack_net').enabled() or (get_option('jack_net').auto() and lib_jacknet.found())
  build_jack_net = true
endif

build_jack_netsource = false
if get_option('jack_netsource').enabled() or (get_option('jack_netsource').auto() and dep_samplerate.found())
  build_jack_netsource = true
endif

opus_support = false
if get_option('opus_support').enabled() or (get_option('opus_support').auto() and dep_opus.found())
  opus_support = true
endif

build_jack_rec = false
if get_option('jack_rec').enabled() or (get_option('jack_rec').auto() and dep_sndfile.found())
  build_jack_rec = true
endif

build_zalsa = false
if get_option('zalsa').enabled() or (
  get_option('zalsa').auto() and dep_alsa.found() and lib_zita_alsa_pcmi.found() and lib_zita_resampler.found()
)
  build_zalsa = true
endif


message('Build alsa_in and alsa_out executables: ' + build_alsa_in_out.to_string())
message('Build jack_net_master and jack_net_slave executables: ' + build_jack_net.to_string())
message('Build jack_netsource executable: ' + build_jack_netsource.to_string())
if build_jack_netsource
  message('Build jack_netsource with opus support: ' + opus_support.to_string())
endif
message('Build jack_rec executable: ' + build_jack_rec.to_string())
message('Build ZALSA internal clients: ' + build_zalsa.to_string())

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
