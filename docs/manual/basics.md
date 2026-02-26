# API Basics

## Serialization
Astra has a built-in serialization and deserialization system. It lives in the `astra::serialization` namespace, and can be included from `astra/serialization/<format>.hpp`.  
Each format can serialize either to a string (JSON/YAML), a byte vector (Binary), or an output stream (all). Deserialization supports the inverse.  
There is also a format conversion system found in `astra/serialization/convert.hpp`, to convert between the different formats in-place.

## Reflection
The primary entrypoint for reflection is `astra::reflection::reflect`, from the `astra/reflection/reflection.hpp` header.  

You can either pass in a pointer to an object you wish to reflect, or a Astra `Var` object, which can be used to examine and modify a field.  
Static members can also be accessed by passing in a `nullptr`, but this is not recommended as it can cause issues with other class objects.  

In order for a class to be reflectable, it must have at least one constructor satisfying the  `std::default_initializable` concept (so, a constructor taking no arguments) and have a public virtual member function named `ASTRA__gettypeid`, returning a `astra::TypeId`. This function will be implemented in the generated reflection code. For convienience, Astra provides the `astra::Reflectable` concept in `astra/reflectable.hpp`, for template constraints to reflectable types. All enums are reflectable by default.

When a value is reflected, it returns a `TypeInfo` object, which is not super useful on its own.
To make it useful, call it's `get` method templated on one of Astra's reflection primitives.  The two most common ones are `Object` and `Enum`, but when working with `Var` objects as mentioned above, you will likely use others.  

`Object` primitives allow you to examine fields and methods, while `Enum` primitives allow you to convert between an enum value and a `std::string`.   The other primitives allow things like checking integer signedness, checking for null pointers, array access, etc. depending on the type.  

When working with methods, it is especially important to ensure correct template arguments, otherwise you will get ugly compile errors.

## Errors
Astra deals with errors by returning `Expected` objects. These can either hold a result value or an error value, which can be checked via the API.  
You can also use the `unwrap` method to return the result and throw an exception if an error value is held.

## A Note on Strings
C `const char*` and STL `std::string_view` objects only reference data owned elsewhere. For this reason, they will be marked as read-only by Astra. Use a `std::string` to own and modify data.
