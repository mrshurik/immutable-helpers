#pragma once

#include <type_traits>
#include <functional>

namespace immu {

template <typename Type>
class ImmutablePtr {
private:
    // can't have reference to void, so change return type to int
    using TypeOrInt = typename std::conditional<std::is_void<Type>::value, int, Type>::type;

public:
    constexpr ImmutablePtr() noexcept : m_ptr(nullptr) {}
    constexpr ImmutablePtr(std::nullptr_t) noexcept : m_ptr(nullptr) {}

    ImmutablePtr(const ImmutablePtr&) = default;

    template <typename Type2>
    ImmutablePtr(const ImmutablePtr<Type2>& ptr,
                     typename std::enable_if<std::is_convertible<Type2*, Type*>::value>::type* = nullptr) noexcept
        : m_ptr(ptr.m_ptr) {}

    explicit
    operator bool() const noexcept { return m_ptr; }

    const Type* get() const noexcept { return m_ptr; }
    const TypeOrInt& operator*() const noexcept { return *m_ptr; }
    const Type* operator->() const noexcept { return m_ptr; }

    friend bool operator==(const ImmutablePtr& left, const ImmutablePtr& right) noexcept {
        return left.m_ptr == right.m_ptr;
    }

    friend bool operator!=(const ImmutablePtr& left, const ImmutablePtr& right) noexcept {
        return !(left == right);
    }

    friend bool operator<(const ImmutablePtr& left, const ImmutablePtr& right) noexcept {
        return left.m_ptr < right.m_ptr;
    }

    template <typename ToType, typename FromType>
    friend ImmutablePtr<ToType> static_pointer_cast(const ImmutablePtr<FromType>&) noexcept;

    template <typename ToType, typename FromType>
    friend ImmutablePtr<ToType> dynamic_pointer_cast(const ImmutablePtr<FromType>&) noexcept;

    template <typename Type2>
    friend ImmutablePtr<Type2> immutable_cast(const Type2* ptr) noexcept;

private:
    static_assert(!std::is_const<Type>::value, "const type is not required");

    template <typename>
    friend class ImmutablePtr;

    ImmutablePtr(const Type* ptr) noexcept : m_ptr(ptr) {}

    const Type* m_ptr;
};

template <typename Type>
inline
ImmutablePtr<Type> immutable_cast(const Type* ptr) noexcept {
    return ptr;
}

template <typename ToType, typename FromType>
inline
ImmutablePtr<ToType> static_pointer_cast(const ImmutablePtr<FromType>& other) noexcept {
    static_assert(std::is_convertible<ToType*, FromType*>::value
                  || std::is_convertible<FromType*, ToType*>::value, "invalid static cast");
    return static_cast<const ToType*>(other.m_ptr);
}

template <typename ToType, typename FromType>
inline
ImmutablePtr<ToType> dynamic_pointer_cast(const ImmutablePtr<FromType>& other) noexcept {
    static_assert(std::is_convertible<ToType*, FromType*>::value
                  || std::is_convertible<FromType*, ToType*>::value, "invalid dynamic cast");
    return dynamic_cast<const ToType*>(other.m_ptr);
}

}

namespace std {

template <typename Type>
struct hash<immu::ImmutablePtr<Type>> : private hash<const Type*> {
    size_t operator()(const immu::ImmutablePtr<Type>& ptr) const noexcept { return hash<const Type*>::operator()(ptr.get()); }
};

}
