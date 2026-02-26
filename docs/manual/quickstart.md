# Quickstart

This guide assumes you are using Meson for your project (Astra's build system). If you aren't, you may need to use pre-compiled binaries or find some way to integrate Meson into your system.  

1. Add Astra as a subproject  
    1. Set up a `astra.wrap` file in your `subprojects` directory. It should look something like this:
	```{code-block}ini
	[wrap-git]
	url = https://github.com/RobotLeopard86/Astra
	depth = 1
	revision = <tag, branch, or commit to clone>
	```
	2. Add these lines to your `meson.build`:
	```{code-block}meson
	astra = subproject('astra', required: true, default_options: ['generator=true'])
	astra_generator = astra.get_variable('astra_generator')
	astra_dep = astra.get_variable('astra_dep')
	```  
2. Configure reflection codegen
	1. Create the custom target. It should look something like this:
	```{code-block}meson
	reflection = custom_target('example.astra.[cpphpp]', output: ['example.astra.hpp', 'example.astra.cpp'], input: ['some_header.hpp'], console: true, command: [astra_generator, '-c', meson.global_build_root(), '-o', meson.current_build_dir(), '-p', 'example', meson.current_source_dir()])
	```
	2. Add the custom target to your target's sources
	3. Add `astra_dep` to your target's dependencies
	4. Not strictly necessary, but recommended: add `-Wno-unknown-attributes` (or equivalent flag) to your extra C++ arguments. This will silence warnings regarding the astra custom attributes
3. Add reflection attributes
	1. Note: for a class to be reflectable, it must satisfy the `astra::reflection::Reflectable` concept.
	2. Add the `[[astra::reflect]]` attribute to your class (or enum), like this:
	```{code-block}cpp
	class [[astra::reflect]] SomeClass {...};
	```
4. Reflect!
	1. Include `astra/reflection/reflection.hpp` and your generated reflection header
	2. Use `astra::reflection::reflect(<pointer>)`
