project('astra', 'c', 'cpp', version: '1.0.0-rc1', meson_version: '>=1.6.0', license: 'Apache-2.0', default_options: ['cpp_std=c++20', 'default_library=static', 'b_vscrt=from_buildtype'])

bt = get_option('buildtype')
cxx = meson.get_compiler('cpp')

# Define compile args for export
add_project_arguments(['-DASTRA_BUILD'], language: 'cpp')

# Get options
example = get_option('example')
generator_cross = get_option('generator_cross')
if meson.is_cross_build() and generator_cross and not ['windows', 'darwin', 'linux'].contains(host_machine.system())
	error('Cannot cross-compile generator to unsupported platform!')
endif

# Build generator
subdir('generator')

# Build library
subdir('src')

# Build example if requested
if example
	subdir('example')
endif

# Install headers
install_subdir('include' / 'astra', install_dir: 'include')
