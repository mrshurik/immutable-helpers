#pragma once

#include <immu/pointer.h>

#include <type_traits>
#include <memory>

namespace immu {

template <typename Type>
class ImmutableDataWeakPtr;

template <typename Type>
class ImmutableDataSharedPtr {
private:
    // can't have reference to void, so change return type to int
    using TypeOrInt = typename std::conditional<std::is_void<Type>::value, int, Type>::type;

public:
    using element_type = typename std::remove_extent<Type>::type;
    using weak_type = ImmutableDataWeakPtr<Type>;

    using SharedPtr = std::shared_ptr<const Type>;

    constexpr ImmutableDataSharedPtr() noexcept = default;

    ImmutableDataSharedPtr(const ImmutableDataSharedPtr&) = default;
    ImmutableDataSharedPtr(ImmutableDataSharedPtr&&) = default;
    ImmutableDataSharedPtr& operator=(const ImmutableDataSharedPtr&) = default;
    ImmutableDataSharedPtr& operator=(ImmutableDataSharedPtr&&) = default;

    constexpr ImmutableDataSharedPtr(std::nullptr_t) noexcept : ImmutableDataSharedPtr() {}

    template <typename Type2>
    ImmutableDataSharedPtr(ImmutableDataSharedPtr<Type2> ptr,
                           typename std::enable_if<std::is_convertible<Type2*, Type*>::value>::type* = nullptr) noexcept
        : m_pointer(std::move(ptr.m_pointer)) {}

    const TypeOrInt& operator*() const noexcept { return *m_pointer; }
    const Type* operator->() const noexcept { return m_pointer.operator->(); }
    const Type* get() const noexcept { return m_pointer.get(); }
    long use_count() const noexcept { return m_pointer.use_count(); }

    ImmutableDataPtr<Type> ptr() const noexcept { return immutable_cast<Type>(get()); }
    const SharedPtr& shared_ptr() const noexcept { return m_pointer; }

    explicit
    operator bool() const noexcept { return bool(m_pointer); }

    void swap(ImmutableDataSharedPtr& other) noexcept { m_pointer.swap(other.m_pointer); }
    void reset() noexcept { m_pointer.reset(); }

    template <typename ToType, typename FromType>
    friend ImmutableDataSharedPtr<ToType> static_pointer_cast(ImmutableDataSharedPtr<FromType>) noexcept;

    template <typename ToType, typename FromType>
    friend ImmutableDataSharedPtr<ToType> dynamic_pointer_cast(ImmutableDataSharedPtr<FromType>) noexcept;

    template <typename Type2>
    friend ImmutableDataSharedPtr<Type2> immutable_cast(std::shared_ptr<const Type2>) noexcept;

    template <typename Type2, typename ...Args>
    friend ImmutableDataSharedPtr<Type2> make_immutable_shared(Args&&...) noexcept;

    friend bool operator==(const ImmutableDataSharedPtr& left, const ImmutableDataSharedPtr& right) noexcept {
        return left.m_pointer == right.m_pointer;
    }

    friend bool operator!=(const ImmutableDataSharedPtr& left, const ImmutableDataSharedPtr& right) noexcept {
        return !(left == right);
    }

    friend bool operator<(const ImmutableDataSharedPtr& left, const ImmutableDataSharedPtr& right) noexcept {
        return left.m_pointer < right.m_pointer;
    }

private:
    static_assert(!std::is_const<element_type>::value, "const type is not required");

    template <typename>
    friend class ImmutableDataSharedPtr;

    ImmutableDataSharedPtr(SharedPtr&& ptr) noexcept : m_pointer(std::move(ptr)) {}

    SharedPtr m_pointer;
};

template <typename Type>
inline
ImmutableDataSharedPtr<Type> immutable_cast(std::shared_ptr<const Type> ptr) noexcept {
    return std::move(ptr);
}

template <typename Type>
class ImmutableDataWeakPtr {
public:
    using element_type = typename std::remove_extent<Type>::type;
    using WeakPtr = std::weak_ptr<const Type>;

    constexpr ImmutableDataWeakPtr() noexcept = default;

    ImmutableDataWeakPtr(const ImmutableDataWeakPtr&) = default;
    ImmutableDataWeakPtr(ImmutableDataWeakPtr&&) = default;
    ImmutableDataWeakPtr& operator=(const ImmutableDataWeakPtr&) = default;
    ImmutableDataWeakPtr& operator=(ImmutableDataWeakPtr&&) = default;

    template <typename Type2>
    ImmutableDataWeakPtr(ImmutableDataWeakPtr<Type2> ptr,
                           typename std::enable_if<std::is_convertible<Type2*, Type*>::value>::type* = nullptr) noexcept
        : m_pointer(std::move(ptr.m_pointer)) {}

    ImmutableDataWeakPtr(const ImmutableDataSharedPtr<Type>& ptr) noexcept
        : m_pointer(ptr.shared_ptr()) {}

    ImmutableDataWeakPtr& operator=(const ImmutableDataSharedPtr<Type>& ptr) noexcept {
        m_pointer = ptr.shared_ptr();
        return *this;
    }

    long use_count() const noexcept { return m_pointer.use_count(); }
    bool expired() const noexcept { return m_pointer.expired(); }

    void swap(ImmutableDataWeakPtr& other) noexcept { m_pointer.swap(other.m_pointer); }
    void reset() noexcept { m_pointer.reset(); }

    ImmutableDataSharedPtr<Type> lock() const noexcept {
        return immutable_cast<Type>(m_pointer.lock());
    }

private:
    static_assert(!std::is_const<element_type>::value, "const type is not required");

    template <typename>
    friend class ImmutableDataWeakPtr;

    WeakPtr m_pointer;
};

template <typename ToType, typename FromType>
inline
ImmutableDataSharedPtr<ToType> static_pointer_cast(ImmutableDataSharedPtr<FromType> ptr) noexcept {
    static_assert(std::is_convertible<ToType*, FromType*>::value
                  || std::is_convertible<FromType*, ToType*>::value, "invalid static cast");
    return static_pointer_cast<const ToType>(std::move(ptr.m_pointer));
}

template <typename ToType, typename FromType>
inline
ImmutableDataSharedPtr<ToType> dynamic_pointer_cast(ImmutableDataSharedPtr<FromType> ptr) noexcept {
    static_assert(std::is_convertible<ToType*, FromType*>::value
                  || std::is_convertible<FromType*, ToType*>::value, "invalid dynamic cast");
    return dynamic_pointer_cast<const ToType>(std::move(ptr.m_pointer));
}

template <typename Type, typename ...Args>
inline
ImmutableDataSharedPtr<Type> make_immutable_shared(Args&& ...args) noexcept {
    return std::make_shared<const Type>(std::forward<Args>(args)...);
}

}

namespace std {

template <typename Type>
struct hash<immu::ImmutableDataSharedPtr<Type>> : private hash<std::shared_ptr<const Type>> {
    size_t operator()(const immu::ImmutableDataSharedPtr<Type>& ptr) const noexcept {
        return hash<std::shared_ptr<const Type>>::operator()(ptr.shared_ptr()); }
};

template <typename Type>
inline
void swap(immu::ImmutableDataSharedPtr<Type>& left, immu::ImmutableDataSharedPtr<Type>& right) noexcept {
    left.swap(right);
}

template <typename Type>
inline
void swap(immu::ImmutableDataWeakPtr<Type>& left, immu::ImmutableDataWeakPtr<Type>& right) noexcept {
    left.swap(right);
}

}
