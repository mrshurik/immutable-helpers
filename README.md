[![Build Status](https://travis-ci.com/mrshurik/immutable-helpers.svg?branch=master)](https://travis-ci.com/mrshurik/immutable-helpers)

# Immutable helpers

A header-only C++ library of type-safe immutable wrappers for data and pointers. Make your code a bit safer!

## Motivation

C++ has no built-in concept of immutable objects. All what we have is const qualifier. But ```const Object& obj``` just means it can't be changed right here while it may be changed at a different place in code where non-const ref is available. Sometimes it's unexpected and leads to nasty bugs.

Say, class Person needs to hold a ref to immutable Name object. How can we explicitly request it? A simple solution?

```C++

template <typename Type>
using Immutable = const Type;

class Person {
    Person(Immutable<Name>& name);
};
```

This looks pretty and explicit but immutability is not enforced. One can pass ```Name&``` and it will be accepted. The solution relies on a human and the compiler won't help us. In order to provide a strong guarantee we need some more ... type-safe wrappers.

## Immutable helpers types

All the wrappers are zero-cost abstractions with no memory and runtime overhead. The implementation is trivial.

### Immutable

It's a "container" for immutable data, holds const Type member. Can be constructed and copied only, no assignment or move. Works best with small objects like std::string or std::tuple when shared ownership is not needed. In interfaces you can use ```const Immutable<Type>&``` directly or get an ```ImmutablePtr<Type>``` from it.

### ImmutablePtr

It's a replacement for plain ```const Type*```. Non-owning pointer, can point to any immutable data. In comparison to ```const Immutable<Type>&``` it doesn't require the data to be allocated inside ```Immutable<Type>``` and it supports polymorphic behavior and pointer conversion for base/derived types. Be careful with object lifetime.

### ImmutableSharedPtr and its ImmutableWeakPtr

It's a replacement for ```shared_ptr<const Type>```. Holds shared ownership and points to an immutable data or a part of immutable data. Can be converted to ```ImmutableWeakPtr<Type>``` or you can get ```ImmutablePtr<Type>``` from it. Please use make_immutable_shared instead of make_shared.

### Unique immutable pointer

There is no one. If you need non-shared immutable data with move semantics you can use ```unique_ptr<Immutable<Type>>```.

### Pointer conversion

Upcast is auto conversion, downcast can be done with ```static_pointer_cast``` or ```dynamic_pointer_cast```.

### Conversion to immutable types

Quite often the data is constructed for us by an external API and is returned as a mutable type. When you're sure that the data is de-facto immutable you can convert it to the immutable type with ```immutable_cast``` function. Try to convert it as early as the data appears in your code.

## License

This project is licensed under the [MIT License](LICENSE.txt)

## Author

Dr. Alexander Bulovyatov. Please share your opinion and ideas to bulovyatov AT g m a i l . c o m
