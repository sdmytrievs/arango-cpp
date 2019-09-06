#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>


//https://doc.qt.io/qtcreator/creator-autotest.html
// https://stackoverflow.com/questions/39574360/google-testing-framework-and-qt
// https://stackoverflow.com/questions/49756620/including-google-tests-in-qt-project

using namespace testing;

TEST(JSONIO, tests)
{
    EXPECT_EQ(1, 1);
    ASSERT_THAT(0, Eq(0));
}
