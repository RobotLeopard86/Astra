# Simple dependencies
cli11_dep = subproject('cli11', required: true).get_variable('CLI11_dep')
inja_dep = subproject('inja', required: true, default_options: {
	'build_tests': false
}).get_variable('inja_dep')
spinners_dep = subproject('spinners-plus-plus', required: true).get_variable('spinners_dep')
deps = [cli11_dep, inja_dep, spinners_dep]

# Validate cross-compilation
generator_cross = get_option('generator')
if meson.is_cross_build() and generator_cross and not ['windows', 'darwin', 'linux'].contains(host_machine.system())
	error('Cannot cross-compile generator to unsupported platform!')
endif

# LLVM/Clang dependencies
if host_machine.system() == 'windows'
	cxx = meson.get_compiler('cpp')
	llvm_win_sp = subproject('llvm-win-' + host_machine.cpu_family())
	deps += llvm_win_sp.get_variable('llvm_dep')
	deps += cxx.find_library('Version', required: true)
	deps += cxx.find_library('ntdll', required: true)
else
	deps += dependency('llvm', version: '>=19.0.0', required: true)
	deps += cxx.find_library('clang-cpp', required: true, dirs: host_machine.system() == 'darwin' ? '/opt/homebrew/opt/llvm/lib' : [])
endif

# Define sources
generator_sources = [
	'main.cpp',
	'files.cpp',
	'parser.cpp',
	'sysinclude.cpp',
	'callback.cpp'
]

# Executable
astra_generator = executable('astragen', generator_sources, dependencies: deps, cpp_args: ['-DPROJECT_VER="' + meson.project_version() + '"'], native: not (meson.is_cross_build() and generator_cross))