#pragma once

#include <utility>
#include <type_traits>

namespace immu {

template <typename>
class ImmutableDataPtr;

template <typename Type>
ImmutableDataPtr<Type> immutable_cast(const Type*) noexcept;

template <typename Type>
class Immutable {
public:
    using value_type = Type;

    // avoid that perfect forwarding is called instead of copy
    Immutable(const Immutable&) = default;
    Immutable(Immutable&) = default;
    Immutable(Immutable&&) = default;

    template <typename ...Args>
    Immutable(Args&& ...args)
        : m_value(std::forward<Args>(args)...) {}

    Immutable& operator=(Immutable&&) = delete;
    Immutable& operator=(const Immutable&) = delete;

    const Type& get() const noexcept { return m_value; }
    ImmutableDataPtr<Type> ptr() const noexcept { return immutable_cast<Type>(&m_value); }

    operator const Type& () const noexcept { return m_value; }

private:
    static_assert(!std::is_const<Type>::value, "const type is not required");

    const Type m_value;
};

}

namespace std {

template <typename Type>
struct hash;

template <typename Type>
struct hash<immu::Immutable<Type>> : private hash<Type> {
    size_t operator()(const immu::Immutable<Type>& obj) const noexcept { return hash<Type>::operator()(obj.get()); }
};

}
