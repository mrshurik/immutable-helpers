#pragma once

#include <utility>

namespace immu {

template <typename>
class ImmutableDataPtr;

template <typename Type>
ImmutableDataPtr<Type> immutable_cast(const Type*) noexcept;

template <typename Type>
class Immutable {
public:
    using value_type = Type;

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
    const Type m_value;
};

}
