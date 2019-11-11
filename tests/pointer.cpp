#include <immu/pointer.h>
#include <immu/immutable.h>

#include <gtest/gtest.h>

#include <type_traits>
#include <utility>
#include <functional>

using namespace immu;

namespace {

struct Base {

    Base() : v(0) {}

    virtual ~Base() {}

    int v;
};

struct Derived : Base {};

}

TEST(pointer, basic) {
    ImmutablePtr<int> np1;
    ImmutablePtr<int> np2 = nullptr;
    EXPECT_EQ(np1, np2);
    std::unique_ptr<Derived> ptr{new Derived};
    ImmutablePtr<Derived> derp = immutable_cast<Derived>(ptr.get());
    ImmutablePtr<Base> bp = derp;
    ImmutablePtr<void> vp = bp;

    auto vp2 = static_pointer_cast<Base>(vp);
    auto derp2 = dynamic_pointer_cast<Derived>(bp);
    EXPECT_EQ(derp2, derp);
    EXPECT_NE(derp2, nullptr);
    EXPECT_TRUE(derp);

    EXPECT_TRUE(std::is_trivially_copyable<decltype(bp)>::value);
    EXPECT_TRUE(std::is_trivially_copy_assignable<decltype(bp)>::value);

    ImmutablePtr<Base> bp2;
    EXPECT_TRUE((bp2 < derp) ^ (derp < bp2));

    EXPECT_EQ(0, bp->v);
    EXPECT_EQ(bp.get(), &*bp);

    Immutable<double> id = 5;
    ImmutablePtr<double> h1p = id.ptr();
    EXPECT_EQ(id.get(), *h1p);

    EXPECT_EQ(std::hash<ImmutablePtr<int>>()(np1), std::hash<const int*>()(np1.get()));
}

