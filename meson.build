project(
  'jack-example-tools',
  ['c', 'cpp'],
  meson_version: '>=0.58.0',
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

dep_jack = dependency('jack')

jack_implementation = ''
jack_implementation = dep_jack.get_variable('jack_implementation')
if jack_implementation == ''
  warning('No compatible jack implementation detected. This may mean conflicting files when installing!')
endif

has_jack1_internal_client = cc.compiles(
  '''
  #include <stdio.h>
  #include <jack/jack.h>
  #include <jack/intclient.h>

  int main (int argc, char *argv[]) {
    const char *client_name;
    jack_client_t *client;
    jack_status_t status;
    jack_intclient_t intclient;

    client_name = "foo";
    client = jack_client_open(client_name, JackNoStartServer, &status);
    jack_internal_client_handle(client, client_name, &status, &intclient);
  }
  '''
)
message('Provides jack1-style jack_internal_client_handle(): ' + has_jack1_internal_client.to_string())

has_jack2_internal_client = cc.compiles(
  '''
  #include <jack/jack.h>
  #include <jack/intclient.h>

  int main (int argc, char *argv[]) {
    const char *client_name;
    jack_client_t *client;
    jack_status_t status;
    jack_intclient_t intclient;

    client_name = "foo";
    client = jack_client_open(client_name, JackNoStartServer, &status);
    intclient = jack_internal_client_handle (client, client_name, &status);
  }
  '''
)
message('Provides jack2-style jack_internal_client_handle(): ' + has_jack2_internal_client.to_string())

lib_jackserver = cc.find_library('jackserver', required: true)
has_jackctl_server_create2 = cc.has_function('jackctl_server_create2', dependencies: lib_jackserver, prefix: '#include <jack/control.h>')

lib_jacknet = cc.find_library('jacknet', required: get_option('jack_net'))

dep_alsa = dependency('alsa', version: '>=1.0.18', required: alsa_required)
dep_opus = dependency('opus', version: '>=0.9.0', required: get_option('opus_support'))
dep_readline = dependency('readline', required: get_option('readline_support'))
dep_samplerate = dependency('samplerate', required: libsamplerate_required)
dep_sndfile = dependency('sndfile', required: get_option('jack_rec'))
dep_threads = dependency('threads')
lib_zita_alsa_pcmi = cc.find_library('zita-alsa-pcmi', required: get_option('zalsa'))
lib_zita_resampler = cc.find_library('zita-resampler', required: get_option('zalsa'))
has_ppoll = cc.has_function('ppoll', prefix: '#define _GNU_SOURCE\n#include <sys/poll.h>')

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

readline_support = false
if get_option('readline_support').enabled() or (get_option('readline_support').auto() and dep_readline.found())
  readline_support = true
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
message('Build jack_transport with readline support: ' + readline_support.to_string())
message('Build ZALSA internal clients: ' + build_zalsa.to_string())

conf_data = configuration_data()
conf_data.set('VERSION', meson.project_version())
conf_data.set('DATE', '2022')

c_args_common = [
  '-D__PROJECT_VERSION__="@0@"'.format(conf_data.get('VERSION')),
]

subdir('tools')
subdir('example-clients')
subdir('man')
