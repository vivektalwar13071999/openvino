// Copyright (C) 2018-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "ngraph/dimension.hpp"

#include "gtest/gtest.h"

using namespace std;
using namespace ngraph;

TEST(dimension, broadcast_merge_static_1_and_10) {
    Dimension result;
    Dimension one(1), ten(10);
    bool success = Dimension::broadcast_merge(result, one, ten);

    EXPECT_TRUE(success);
    EXPECT_EQ(result, ten);
}

TEST(dimension, broadcast_merge_static_1_5_and_10_15) {
    Dimension result;
    Dimension one(1, 5), ten(10, 15);
    bool success = Dimension::broadcast_merge(result, one, ten);

    EXPECT_TRUE(success);
    EXPECT_EQ(result, ten);
}

TEST(dimension, broadcast_merge_static_1_12_and_10_15) {
    Dimension result;
    Dimension one(1, 12), ten(10, 15);
    bool success = Dimension::broadcast_merge(result, one, ten);

    EXPECT_TRUE(success);
    EXPECT_EQ(result, ten);
}

TEST(dimension, broadcast_merge_static_7_12_and_10_15) {
    Dimension result;
    Dimension one(7, 12), ten(10, 15);
    bool success = Dimension::broadcast_merge(result, one, ten);

    EXPECT_TRUE(success);
    EXPECT_EQ(result, Dimension(10, 12));
}

TEST(dimension, broadcast_merge_static_0_12_and_1_15) {
    Dimension result;
    Dimension one(0, 12), ten(1, 15);
    bool success = Dimension::broadcast_merge(result, one, ten);

    EXPECT_TRUE(success);
    EXPECT_EQ(result, Dimension(0, 15));
}
