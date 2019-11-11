#include <immu/immutable.h>

#include <gtest/gtest.h>

#include <type_traits>
#include <utility>
#include <functional>

using namespace immu;

TEST(immutable, basic) {
    using Pair = std::pair<int, float>;

    Immutable<Pair> a(1, 2.0);
    const auto a1 = a;
    auto a2 = a1;
    auto a3 = Immutable<Pair>{3, 4};
    Pair v = a1.get();

    Immutable<double> h1 = 1;
    Immutable<double> h2 = 1;
    EXPECT_EQ(h1, h2);
    EXPECT_LT(h1, 2);

    ImmutablePtr<Pair> ptr = a3.ptr();
    EXPECT_EQ(ptr.get(), &a3.get());

    EXPECT_TRUE(std::is_trivially_copyable<decltype(h1)>::value);
    EXPECT_FALSE(std::is_copy_assignable<decltype(h1)>::value);
    EXPECT_FALSE(std::is_move_assignable<decltype(h1)>::value);

    EXPECT_EQ(std::hash<Immutable<std::string>>()("hi"), std::hash<std::string>()("hi"));
}
