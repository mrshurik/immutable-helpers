#include <immu/shared_ptr.h>

#include <gtest/gtest.h>

#include <type_traits>
#include <utility>
#include <memory>

using namespace immu;

namespace {

struct Base {

    Base() : v(0) {}

    virtual ~Base() {}

    int v;
};

struct Derived : Base {};

}

TEST(shared, basic) {
    ImmutableSharedPtr<Base> np1;
    ImmutableSharedPtr<Base> np2 = nullptr;
    EXPECT_EQ(np1, np2);
    EXPECT_EQ(np1, nullptr);

    auto sp = std::make_shared<const Derived>();
    ImmutableSharedPtr<Base> dp1 = immutable_cast<Derived>(sp);
    EXPECT_EQ(sp, dp1.shared_ptr());

    ImmutableSharedPtr<Derived> dp2 = dynamic_pointer_cast<Derived>(dp1);
    EXPECT_EQ(dp1, dp2);
    ImmutableSharedPtr<void> vp = dp1;
    dp2 = static_pointer_cast<Derived>(vp);
    EXPECT_LT(1, dp1.use_count());

    auto bp2 = make_immutable_shared<Base>();
    auto mbp2 = std::move(bp2);
    EXPECT_NE(nullptr, mbp2);
    EXPECT_EQ(1, mbp2.use_count());
    bp2.reset();
    EXPECT_EQ(nullptr, bp2);
    bp2 = std::move(mbp2);

    EXPECT_EQ(0, bp2->v);
    ImmutablePtr<Base> bp2ptr = bp2.ptr();
    EXPECT_EQ(bp2ptr.get(), bp2.get());
    EXPECT_EQ(bp2.get(), &*bp2);

    EXPECT_TRUE((bp2 < dp1) ^ (dp1 < bp2));
    auto dp1c = dp1;
    auto bp2c = bp2;
    dp1.swap(bp2);
    EXPECT_EQ(dp1c, bp2);
    EXPECT_EQ(bp2c, dp1);

    EXPECT_EQ(std::hash<ImmutableSharedPtr<Derived>>()(dp2),
              std::hash<std::shared_ptr<const Derived>>()(dp2.shared_ptr()));
}

TEST(shared, weak_ptr) {
    ImmutableWeakPtr<int> wp1;

    ImmutableSharedPtr<Derived> spd = make_immutable_shared<Derived>();
    ImmutableWeakPtr<Derived> wpd = spd;
    ImmutableWeakPtr<Base> wpb = wpd;
    EXPECT_EQ(1u, wpb.use_count());
    EXPECT_FALSE(wpb.expired());

    ImmutableSharedPtr<Derived> spd2 = wpd.lock();
    EXPECT_TRUE(spd2);
    EXPECT_EQ(spd, spd2);

    auto wpd2 = wpd;
    wpd.reset();
    EXPECT_EQ(wpd.lock(), ImmutableSharedPtr<Derived>());

    wpd.swap(wpd2);
    spd2 = wpd.lock();
    EXPECT_TRUE(spd2);

    EXPECT_EQ(2u, wpb.use_count());
    spd2.reset();
    spd.reset();
    EXPECT_EQ(0, wpb.use_count());
    EXPECT_TRUE(wpb.expired());
    ImmutableSharedPtr<Base> sp0 = wpb.lock();
    EXPECT_EQ(sp0, ImmutableSharedPtr<Base>());
}
