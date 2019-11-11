#pragma once

#include <immu/pointer.h>

#include <type_traits>
#include <memory>

namespace immu {

template <typename Type>
class ImmutableWeakPtr;

template <typename Type>
class ImmutableSharedPtr {
private:
    // can't have reference to void, so change return type to int
    using TypeOrInt = typename std::conditional<std::is_void<Type>::value, int, Type>::type;

public:
    using element_type = typename std::remove_extent<Type>::type;
    using weak_type = ImmutableWeakPtr<Type>;

    using SharedPtr = std::shared_ptr<const Type>;

    constexpr ImmutableSharedPtr() noexcept = default;

    ImmutableSharedPtr(const ImmutableSharedPtr&) = default;
    ImmutableSharedPtr(ImmutableSharedPtr&&) = default;
    ImmutableSharedPtr& operator=(const ImmutableSharedPtr&) = default;
    ImmutableSharedPtr& operator=(ImmutableSharedPtr&&) = default;

    constexpr ImmutableSharedPtr(std::nullptr_t) noexcept : ImmutableSharedPtr() {}

    template <typename Type2>
    ImmutableSharedPtr(ImmutableSharedPtr<Type2> ptr,
                           typename std::enable_if<std::is_convertible<Type2*, Type*>::value>::type* = nullptr) noexcept
        : m_pointer(std::move(ptr.m_pointer)) {}

    const TypeOrInt& operator*() const noexcept { return *m_pointer; }
    const Type* operator->() const noexcept { return m_pointer.operator->(); }
    const Type* get() const noexcept { return m_pointer.get(); }
    long use_count() const noexcept { return m_pointer.use_count(); }

    ImmutablePtr<Type> ptr() const noexcept { return immutable_cast<Type>(get()); }
    const SharedPtr& shared_ptr() const noexcept { return m_pointer; }

    explicit
    operator bool() const noexcept { return bool(m_pointer); }

    void swap(ImmutableSharedPtr& other) noexcept { m_pointer.swap(other.m_pointer); }
    void reset() noexcept { m_pointer.reset(); }

    template <typename ToType, typename FromType>
    friend ImmutableSharedPtr<ToType> static_pointer_cast(ImmutableSharedPtr<FromType>) noexcept;

    template <typename ToType, typename FromType>
    friend ImmutableSharedPtr<ToType> dynamic_pointer_cast(ImmutableSharedPtr<FromType>) noexcept;

    template <typename Type2>
    friend ImmutableSharedPtr<Type2> immutable_cast(std::shared_ptr<const Type2>) noexcept;

    template <typename Type2, typename ...Args>
    friend ImmutableSharedPtr<Type2> make_immutable_shared(Args&&...) noexcept;

    friend bool operator==(const ImmutableSharedPtr& left, const ImmutableSharedPtr& right) noexcept {
        return left.m_pointer == right.m_pointer;
    }

    friend bool operator!=(const ImmutableSharedPtr& left, const ImmutableSharedPtr& right) noexcept {
        return !(left == right);
    }

    friend bool operator<(const ImmutableSharedPtr& left, const ImmutableSharedPtr& right) noexcept {
        return left.m_pointer < right.m_pointer;
    }

private:
    static_assert(!std::is_const<element_type>::value, "const type is not required");

    template <typename>
    friend class ImmutableSharedPtr;

    ImmutableSharedPtr(SharedPtr&& ptr) noexcept : m_pointer(std::move(ptr)) {}

    SharedPtr m_pointer;
};

template <typename Type>
inline
ImmutableSharedPtr<Type> immutable_cast(std::shared_ptr<const Type> ptr) noexcept {
    return std::move(ptr);
}

template <typename Type>
class ImmutableWeakPtr {
public:
    using element_type = typename std::remove_extent<Type>::type;
    using WeakPtr = std::weak_ptr<const Type>;

    constexpr ImmutableWeakPtr() noexcept = default;

    ImmutableWeakPtr(const ImmutableWeakPtr&) = default;
    ImmutableWeakPtr(ImmutableWeakPtr&&) = default;
    ImmutableWeakPtr& operator=(const ImmutableWeakPtr&) = default;
    ImmutableWeakPtr& operator=(ImmutableWeakPtr&&) = default;

    template <typename Type2>
    ImmutableWeakPtr(ImmutableWeakPtr<Type2> ptr,
                           typename std::enable_if<std::is_convertible<Type2*, Type*>::value>::type* = nullptr) noexcept
        : m_pointer(std::move(ptr.m_pointer)) {}

    ImmutableWeakPtr(const ImmutableSharedPtr<Type>& ptr) noexcept
        : m_pointer(ptr.shared_ptr()) {}

    ImmutableWeakPtr& operator=(const ImmutableSharedPtr<Type>& ptr) noexcept {
        m_pointer = ptr.shared_ptr();
        return *this;
    }

    long use_count() const noexcept { return m_pointer.use_count(); }
    bool expired() const noexcept { return m_pointer.expired(); }

    void swap(ImmutableWeakPtr& other) noexcept { m_pointer.swap(other.m_pointer); }
    void reset() noexcept { m_pointer.reset(); }

    ImmutableSharedPtr<Type> lock() const noexcept {
        return immutable_cast<Type>(m_pointer.lock());
    }

private:
    static_assert(!std::is_const<element_type>::value, "const type is not required");

    template <typename>
    friend class ImmutableWeakPtr;

    WeakPtr m_pointer;
};

template <typename ToType, typename FromType>
inline
ImmutableSharedPtr<ToType> static_pointer_cast(ImmutableSharedPtr<FromType> ptr) noexcept {
    static_assert(std::is_convertible<ToType*, FromType*>::value
                  || std::is_convertible<FromType*, ToType*>::value, "invalid static cast");
    return static_pointer_cast<const ToType>(std::move(ptr.m_pointer));
}

template <typename ToType, typename FromType>
inline
ImmutableSharedPtr<ToType> dynamic_pointer_cast(ImmutableSharedPtr<FromType> ptr) noexcept {
    static_assert(std::is_convertible<ToType*, FromType*>::value
                  || std::is_convertible<FromType*, ToType*>::value, "invalid dynamic cast");
    return dynamic_pointer_cast<const ToType>(std::move(ptr.m_pointer));
}

template <typename Type, typename ...Args>
inline
ImmutableSharedPtr<Type> make_immutable_shared(Args&& ...args) noexcept {
    return std::make_shared<const Type>(std::forward<Args>(args)...);
}

}

namespace std {

template <typename Type>
struct hash<immu::ImmutableSharedPtr<Type>> : private hash<std::shared_ptr<const Type>> {
    size_t operator()(const immu::ImmutableSharedPtr<Type>& ptr) const noexcept {
        return hash<std::shared_ptr<const Type>>::operator()(ptr.shared_ptr()); }
};

template <typename Type>
inline
void swap(immu::ImmutableSharedPtr<Type>& left, immu::ImmutableSharedPtr<Type>& right) noexcept {
    left.swap(right);
}

template <typename Type>
inline
void swap(immu::ImmutableWeakPtr<Type>& left, immu::ImmutableWeakPtr<Type>& right) noexcept {
    left.swap(right);
}

}
