#include <limits>

#include <gtest/gtest.h>

#include <Utils/SafeAddress.h>

namespace
{

constexpr auto minAddress = (std::numeric_limits<std::uintptr_t>::min)();
constexpr auto maxAddress = (std::numeric_limits<std::uintptr_t>::max)();

class SafeAddress_AddressTest : public testing::TestWithParam<std::uintptr_t> {};

TEST_P(SafeAddress_AddressTest, AfterConstructionHasAddressPassedToTheConstructor) {
    EXPECT_EQ(SafeAddress{ GetParam() }.as<std::uintptr_t>(), GetParam());
}

TEST_P(SafeAddress_AddressTest, ReturnsCorrectAddressWhenConvertedToReturnAddress) {
    EXPECT_EQ(static_cast<std::uintptr_t>(SafeAddress{ GetParam() }.asReturnAddress()), GetParam());
}

TEST_P(SafeAddress_AddressTest, ReturnsCorrectAddressWhenConvertedToPointer) {
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(SafeAddress{ GetParam() }.as<int*>()), GetParam());
}

INSTANTIATE_TEST_SUITE_P(, SafeAddress_AddressTest, testing::Values(minAddress, 123, maxAddress));

TEST(SafeAddressTest, AddingPositiveOffsetToZeroAddressDoesNothing) {
    EXPECT_EQ(SafeAddress{ 0 }.add(1'000).as<std::uintptr_t>(), 0);
}

TEST(SafeAddressTest, AddingZeroOffsetToZeroAddressDoesNothing) {
    EXPECT_EQ(SafeAddress{ 0 }.add(0).as<std::uintptr_t>(), 0);
}

TEST(SafeAddressTest, AddingNegativeOffsetToZeroAddressDoesNothing) {
    EXPECT_EQ(SafeAddress{ 0 }.add(-1'000).as<std::uintptr_t>(), 0);
}

TEST(SafeAddressTest, AddingZeroOffsetToAnAddressDoesNothing) {
    EXPECT_EQ(SafeAddress{ 123'456 }.add(0).as<std::uintptr_t>(), 123'456);
}

TEST(SafeAddressTest, AddingPositiveOffsetCanWrapAround) {
    EXPECT_EQ(SafeAddress{ maxAddress }.add(1).as<std::uintptr_t>(), minAddress);
}

TEST(SafeAddressTest, AddingNegativeOffsetCanWrapAround) {
    EXPECT_EQ(SafeAddress{ 10 }.add(-11).as<std::uintptr_t>(), maxAddress);
}

TEST(SafeAddressTest, CanBeDereferenced) {
    int* ptr1 = nullptr;
    int** ptr2 = &ptr1;
    int*** ptr3 = &ptr2;
    int**** ptr4 = &ptr3;

    EXPECT_EQ(SafeAddress{ std::uintptr_t(ptr4) }.deref<2>().as<std::uintptr_t>(), std::uintptr_t(&ptr1));
}

TEST(SafeAddressTest, DereferenceStopsWhenReachedZeroAddress) {
    int*** ptr3 = nullptr;
    int**** ptr4 = &ptr3;
    int***** ptr5 = &ptr4;

    EXPECT_EQ(SafeAddress{ std::uintptr_t(ptr5) }.deref<10>().as<std::uintptr_t>(), 0);
}

TEST(SafeAddressTest, DereferencingZeroAddressDoesNothing) {
    EXPECT_EQ(SafeAddress{ 0 }.deref<2>().as<std::uintptr_t>(), 0);
    EXPECT_EQ(SafeAddress{ 0 }.deref().as<std::uintptr_t>(), 0);
}

TEST(SafeAddressTest, NonTemplateDerefMethodDereferencesOnce) {
    int* ptr1 = nullptr;
    int** ptr2 = &ptr1;
    int*** ptr3 = &ptr2;

    EXPECT_EQ(SafeAddress{ std::uintptr_t(ptr3) }.deref().as<std::uintptr_t>(), std::uintptr_t(&ptr1));
}

TEST(SafeAddressTest, ConvertingZeroAddressToAbsoluteDoesNothing) {
    EXPECT_EQ(SafeAddress{ 0 }.abs().as<std::uintptr_t>(), 0);
}

class SafeAddress_RelativeToAbsoluteTest : public testing::TestWithParam<std::int32_t> {};

TEST_P(SafeAddress_RelativeToAbsoluteTest, RelativeAddressIsCorrectlyConvertedToAbsolute) {
    const std::int32_t relativeJumpOffset = GetParam();
    EXPECT_EQ(SafeAddress{ std::uintptr_t(&relativeJumpOffset) }.abs().as<std::uintptr_t>(), std::uintptr_t(&relativeJumpOffset) + sizeof(std::int32_t) + relativeJumpOffset);
}

INSTANTIATE_TEST_SUITE_P(, SafeAddress_RelativeToAbsoluteTest, testing::Values(-1000, 0, 1000));

}
