# Generate reflection info
reflection = custom_target('example.astra.[cpphpp]', output: ['example.astra.hpp', 'example.astra.cpp'], input: ['data' / 'car.hpp', 'data' / 'colors.hpp', 'data' / 'suv.hpp', 'data' / 'complicatedtype.hpp', 'data' / 'substitutedtype.hpp'], console: true,
	command: [astra_generator, '-c', meson.global_build_root(), '-o', meson.current_build_dir(), '-n', 'example', '-i', meson.current_source_dir() / 'data'])

# Create program
executable('astra_example', sources: [
	'main.cpp',
	'data' / 'car.hpp',
	'data' / 'colors.hpp',
	'data' / 'suv.hpp',
	'data' / 'complicatedtype.hpp',
	'data' / 'substitutedtype.hpp',
	reflection
], include_directories: include_directories('data'), dependencies: astra_dep)
