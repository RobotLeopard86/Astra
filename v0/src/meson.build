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
astra_sources_v0 = [
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
includes_v0 = include_directories('..' / 'include')

# Library
libastra_v0 = library('astra', astra_sources_v0, include_directories: [includes_v0, include_directories('.')], dependencies: [nlohmann_json_dep, reflex_dep], pic: true)

# Dependency
astra_dep_v0 = declare_dependency(link_with: libastra_v0, include_directories: includes_v0)
