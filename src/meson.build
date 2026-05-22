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

# RE-flex
reflex = subproject('reflex', required: true, default_options: {
	'default_library': 'static',
	'cpp_std': 'c++17'
})
reflex_exe = reflex.get_variable('reflex')
reflex_dep = reflex.get_variable('reflex_dep')

# Generate lexer files
json_lex = custom_target('lexer_json.yy.[cpph]', 
	output: ['lexer_json.yy.h', 'lexer_json.yy.cpp'],
	input: 'json.l',
	command: [reflex_exe, '@INPUT@', '--outfile=' + meson.current_build_dir() / 'lexer_json.yy.cpp', '--header-file=' + meson.current_build_dir() / 'lexer_json.yy.h'],
	build_always_stale: false,
	build_by_default: true
)
yaml_lex = custom_target('lexer_yaml.yy.[cpph]', 
	output: ['lexer_yaml.yy.h', 'lexer_yaml.yy.cpp'],
	input: 'yaml.l',
	command: [reflex_exe, '@INPUT@', '--outfile=' + meson.current_build_dir() / 'lexer_yaml.yy.cpp', '--header-file=' + meson.current_build_dir() / 'lexer_yaml.yy.h'],
	build_always_stale: false,
	build_by_default: true
)

# Define sources
astra_sources = [
	'binary.cpp',
	'box.cpp',
	'err_helper.cpp',
	'json.cpp',
	'parser_json.cpp',
	'parser_yaml.cpp',
	'reflection.cpp',
	'stringify.cpp',
	'var.cpp',
	'yaml.cpp',
	json_lex,
	yaml_lex
]

# Define includes
includes = include_directories('..' / 'include')

# Library
libastra = library('astra', astra_sources, include_directories: [includes, include_directories('.')], dependencies: [nlohmann_json_dep, reflex_dep], pic: true)

# Dependency
astra_dep = declare_dependency(link_with: libastra, include_directories: includes)
