# Attributes

Astra has 3 attributes available to customize the reflection experience.

## `[[astra::reflect]]`
This attribute marks a `class` or `enum` as reflectable. However, it also supports some options:
* `base` - Include methods and fields from superclass (provided it is reflectable)
* `data` - Include fields
* `func` - Include methods
* `nonPublic` - Include `private` and `protected` members
* `all` - All options combined  

If no argument is passed, then `data` is assumed. You can also specify multiple arguments like this:
```cpp
class [[astra::reflect("data", "func")]] SomeClass {...};
```  

Also, if using the `nonPublic` option (and by extension `all`, since `nonPublic` is included), you must additionally include the `astra/types/type_actions.hpp` header and add this line to a `private:` block in your class:
```cpp
friend struct astra::TypeActions<YourClassNameHere>;
```  
Otherwise, you will get compile errors in the generated code telling you that the private fields referenced are inaccessible.

## `[[astra::alias]]`
This attribute lets you refer to something in reflection by a different name. For example:
```cpp
class [[astra::reflect]] Hippo {
	[[astra::alias("hungry")]] bool isHungry;
};
```  
Now when using reflection, you call it `hungry` instead of `isHungry`.  
This attribute works on:
* Non-static data members
* Methods
* Classes

## `[[astra::ignore]]`
This attribute lets you exclude a member in a reflectable class from reflection. For example:
```cpp
class [[astra::reflect]] Person {
	std::string name;
	[[astra::ignore]] int creditScore;
};
```  
In this example, you can use `name` in reflection, but not `creditScore`. In fact, trying to access `creditScore` will cause a runtime error and say that it wasn't found. Be mindful of this.