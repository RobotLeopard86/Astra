# nhlomann json
nlohmann_json_dep = subproject('nlohmannjson', required: true).get_variable('nlohmann_json_dep')

# yaml-cpp
yaml_cpp_dep = subproject('yaml-cpp', required: true, default_options: {
	'contrib': true,
	'tools': false,
	'tests': 'disabled'
}).get_variable('yaml_cpp_dep').as_link_whole()

# Jaguar
jaguar_dep = subproject('jaguar', required: true, default_options: ['testing=false']).get_variable('libjaguar_static_dep').as_link_whole()

# dbghelp.lib (Windows)
dbghelp_dep = cxx.find_library('dbghelp.lib', required: host_machine.system() == 'windows')

# Define sources
astra_sources = [
	'binary.cpp',
	'box.cpp',
	'convert.cpp',
	'err_helper.cpp',
	'json.cpp',
	'reflection.cpp',
	'stringify.hpp',
	'type_table.cpp',
	'var.cpp',
	'varserialize.cpp',
	'yaml.cpp'
]

# Generate container substitute reflection info
substitute_data = custom_target('astrasubstitutes.astra.[cpphpp]', output: ['astrasubstitutes.astra.hpp', 'astrasubstitutes.astra.cpp'], input: ['..' / 'include' / 'astra' / 'serialized_substitute.hpp'], console: true,
	command: [astra_generator, '-c', meson.global_build_root(), '-o', meson.current_build_dir(), '-n', 'astrasubstitutes', '@INPUT@'])
astra_sources += substitute_data

# Define includes
includes = include_directories('..' / 'include')

# Library
libastra = library('astra', astra_sources, include_directories: includes, dependencies: [nlohmann_json_dep, yaml_cpp_dep, jaguar_dep, dbghelp_dep], pic: true, install: true)

# Dependency
astra_dep = declare_dependency(link_with: libastra, include_directories: includes, sources: substitute_data[0], compile_args: (get_option('default_library') == 'static' ? ['-DASTRA_STATICLIB'] : []), dependencies: [nlohmann_json_dep, yaml_cpp_dep.partial_dependency(includes: true, compile_args: true), jaguar_dep.partial_dependency(includes: true, compile_args: true)])
