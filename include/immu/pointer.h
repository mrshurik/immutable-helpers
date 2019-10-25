#pragma once

#include <type_traits>

namespace immu {

template <typename Type>
class ImmutableDataPtr {
private:
    // can't have reference to void, so change return type to int
    using TypeOrInt = typename std::conditional<std::is_void<Type>::value, int, Type>::type;

public:
    constexpr ImmutableDataPtr()
        : m_ptr(nullptr) {}

    constexpr ImmutableDataPtr(std::nullptr_t)
        : m_ptr(nullptr) {}

    ImmutableDataPtr(const ImmutableDataPtr&) = default;

    template <typename Type2>
    ImmutableDataPtr(const ImmutableDataPtr<Type2>& ptr,
                     typename std::enable_if<std::is_convertible<Type2*, Type*>::value>::type* = nullptr) noexcept
        : m_ptr(ptr.m_ptr) {}

    template <typename Type2>
    explicit
    ImmutableDataPtr(const ImmutableDataPtr<Type2>& ptr,
                     typename std::enable_if<std::is_convertible<Type*, Type2*>::value>::type* = nullptr) noexcept
        : m_ptr(static_cast<const Type*>(ptr.m_ptr)) {}

    explicit
    operator bool() const noexcept { return m_ptr; }

    const Type* get() const noexcept { return m_ptr; }

    const TypeOrInt& operator*() const noexcept { return *m_ptr; }

    const Type* operator->() const noexcept { return m_ptr; }

    friend bool operator==(const ImmutableDataPtr& left, const ImmutableDataPtr& right) noexcept {
        return left.m_ptr == right.m_ptr;
    }

    friend bool operator!=(const ImmutableDataPtr& left, const ImmutableDataPtr& right) noexcept {
        return !(left == right);
    }

private:
    template <typename>
    friend class ImmutableDataPtr;

    template <typename Type2>
    friend ImmutableDataPtr<Type2> immutable_cast(const Type2* ptr) noexcept;

    explicit ImmutableDataPtr(const Type* ptr) noexcept
        : m_ptr(ptr) {}

    const Type* m_ptr;
};

template <typename Type>
inline ImmutableDataPtr<Type> immutable_cast(const Type* ptr) noexcept {
    return ImmutableDataPtr<Type>(ptr);
}

}
