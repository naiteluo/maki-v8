#include "gtest/gtest.h"
#include "Demo.h"

TEST(demoTest, test1) {
    //arrange
    //act
    //assert
    EXPECT_EQ (Demo::say (0),  0);
    EXPECT_EQ (Demo::say (10), 20);
    EXPECT_EQ (Demo::say (50), 100);
}