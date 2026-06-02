# Quickstart

This guide assumes you are using Meson for your project (Astra's build system). If you aren't, you may need to use pre-compiled binaries or some form of external project mechanism.

1. Add Astra as a subproject  
    1. Set up a `astra.wrap` file in your `subprojects` directory. It should look something like this:
	```{code-block}
	[wrap-git]
	url = https://github.com/RobotLeopard86/Astra
	depth = 1
	revision = <tag, branch, or commit hash to clone>
	```
	2. Add these lines to your `meson.build`:
	```{code-block}
	astra = subproject('astra', required: true)
	astra_generator = astra.get_variable('astra_generator')
	astra_dep = astra.get_variable('astra_dep')
	```  
2. Configure reflection codegen
	1. Create the custom target. It should look something like this:
	```{code-block}
	reflection = custom_target('example.astra.[cpphpp]', output: ['example.astra.hpp', 'example.astra.cpp'],
		input: ['some_header.hpp'], console: true, command: [astra_generator, '-c', meson.global_build_root(), '-o', 
		meson.current_build_dir(), '-p', 'example', meson.current_source_dir()])
	```
	2. Add the custom target to your library/executable's sources
	3. Add `astra_dep` to your library/executable's dependencies
3. Setup reflection in code
	1. Include the `astra/setup.hpp` header in order to gain access to the reflection macros
	2. Check that your enum/class conforms to the `astra::Reflectable` concept (must be either an enum, a primitive, or a default-initializable class)
	3. Mark an enum/class as reflectable by adding the `ASTRA_REFLECT` macro before the name and by making it inherit from `AstraReflectBase` it it's a class
		1. Note: your class does not need to directly inherit from `AstraReflectBase` if it inherits from a class that already does
	4. For classes: add reflection configuration data by adding `ASTRASETUP(<your class name goes here>)` within a `public` section of your class
4. Reflect!
	1. Include `astra/reflection.hpp` and your generated reflection header in the file where you want to do reflection
	2. Use `astra::reflect(<pointer to reflectable object>)` to get an `astra::TypeInfo` object, which allows you to access reflection functionality.
