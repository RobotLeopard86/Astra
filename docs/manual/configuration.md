# Configuration

Astra has three macros defined in the `setup.hpp` file that configure how the generator will reflect a given class (only `ASTRA_REFLECT` can be used with enums). When not generating reflection data, these macros evaluate to nothing and are thus ignored. The attributes are as follows:

## `ASTRA_REFLECT`
This attribute marks a class (or enum) as reflectable and will trigger the generator to generate reflection information for the class. If this macro is not found, the class will not be reflected even if it has other macros related to reflection.

## `ASTRA_IGNORE`
This attribute tells the generator to ignore a class member field or method; it will thus not be accessible via the reflection interface and will not be used in serialization or deserialization. Useful for hiding private fields.

## `ASTRA_ALIAS(...)`
This attribute tells the generator to rename a class member field or method in the reflection interface. It will thus not be accessible via its true name using the reflection or serialization APIs, only via the specified name. Place the alias name in quotes within the alias definition, like so: `ASTRA_ALIAS("my_alias") int someField;`