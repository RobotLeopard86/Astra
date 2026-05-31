# Simple dependencies
cli11_dep = subproject('cli11', required: true).get_variable('CLI11_dep')
inja_dep = subproject('inja', required: true, default_options: {
	'build_tests': false
}).get_variable('inja_dep')
spinners_dep = subproject('spinners-plus-plus', required: true).get_variable('spinners_dep')

# LLVM/Clang dependencies
cxx = meson.get_compiler('cpp')
llvm_dep = dependency('llvm', version: '>=19.0.0', required: true)
clang_dep = cxx.find_library('clang-cpp', required: true, dirs: host_machine.system() == 'darwin' ? '/opt/homebrew/opt/llvm/lib' : [])

deps = [cli11_dep, inja_dep, spinners_dep, llvm_dep, clang_dep]

# Windows libraries
if host_machine.system() == 'windows'
	cxx = meson.get_compiler('cpp')
	deps += cxx.find_library('Version', required: true)
	deps += cxx.find_library('ntdll', required: true)
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
astra_generator = executable('astragen', generator_sources, dependencies: deps, cpp_args: ['-DPROJECT_VER="' + meson.project_version() + '"'])