#pragma once

#include <immu/pointer.h>

#include <utility>
#include <type_traits>
#include <functional>

namespace immu {

template <typename Type>
class Immutable {
public:
    using value_type = Type;

    // avoid override of copy/move ctor
    template <typename Arg, typename =
              typename std::enable_if<
                  std::is_same<Immutable,
                               typename std::remove_cv<typename std::remove_reference<Arg>::type>::type
                               >::value>::type>
    Immutable(Arg&& arg)
        : m_value(std::forward<Arg>(arg)) {}

    template <typename ...Args>
    Immutable(Args&& ...args)
        : m_value(std::forward<Args>(args)...) {}

    const Type& get() const noexcept { return m_value; }
    ImmutablePtr<Type> ptr() const noexcept { return immutable_cast<Type>(&m_value); }

    operator const Type& () const noexcept { return m_value; }

private:
    static_assert(!std::is_const<Type>::value, "const type is not required");

    const Type m_value;
};

}

namespace std {

template <typename Type>
struct hash<immu::Immutable<Type>> : private hash<Type> {
    size_t operator()(const immu::Immutable<Type>& obj) const noexcept { return hash<Type>::operator()(obj.get()); }
};

}
