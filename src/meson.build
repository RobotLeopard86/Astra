# nhlomann json
nlohmann_json_dep = subproject('nlohmannjson', required: true).get_variable('nlohmann_json_dep')

# yaml-cpp
yaml_cpp_dep = subproject('yaml-cpp', required: true, default_options: {
	'contrib': true,
	'tools': false,
	'tests': 'disabled'
}).get_variable('yaml_cpp_dep')

# Jaguar
jaguar_dep = subproject('jaguar', required: true).get_variable('libjaguar_dep')

# Define sources
astra_sources = [
	
]

# Define includes
includes = include_directories('..' / 'include')

# Library
libastra = both_libraries('astra', astra_sources, include_directories: [includes, include_directories('.')], dependencies: [nlohmann_json_dep, yaml_cpp_dep, jaguar_dep.as_static(recursive: true).as_link_whole()], pic: true)

# Dependency
astra_dep = declare_dependency(link_with: libastra, include_directories: includes)