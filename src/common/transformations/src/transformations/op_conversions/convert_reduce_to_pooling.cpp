// Copyright (C) 2018-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "transformations/op_conversions/convert_reduce_to_pooling.hpp"

#include "itt.hpp"

ngraph::pass::ConvertReduceMeanToPooling::ConvertReduceMeanToPooling() {
    MATCHER_SCOPE(ConvertReduceMeanToPooling);
    MATCHER_SCOPE_ENABLE(ConvertReduceMeanToPooling);
    auto m = std::make_shared<ngraph::pattern::Matcher>(
        ngraph::pattern::wrap_type<opset1::ReduceMean>(
            {pattern::any_input(pattern::has_static_shape()), pattern::wrap_type<opset1::Constant>()},
            pattern::has_static_shape()),
        matcher_name);
    register_matcher(m, convert_reduce_to_pooling<opset1::ReduceMean>());
}
ngraph::pass::ConvertReduceMaxToPooling::ConvertReduceMaxToPooling() {
    MATCHER_SCOPE(ConvertReduceMaxToPooling);
    MATCHER_SCOPE_ENABLE(ConvertReduceMaxToPooling);
    auto m = std::make_shared<ngraph::pattern::Matcher>(
        ngraph::pattern::wrap_type<opset1::ReduceMax>(
            {pattern::any_input(pattern::has_static_shape()), pattern::wrap_type<opset1::Constant>()},
            pattern::has_static_shape()),
        matcher_name);
    register_matcher(m, convert_reduce_to_pooling<opset1::ReduceMax>());
}
ngraph::pass::ConvertReduceSumToPooling::ConvertReduceSumToPooling() {
    MATCHER_SCOPE(ConvertReduceSumToPooling);
    MATCHER_SCOPE_ENABLE(ConvertReduceSumToPooling);
    auto m = std::make_shared<ngraph::pattern::Matcher>(
        ngraph::pattern::wrap_type<opset1::ReduceSum>(
            {pattern::any_input(pattern::has_static_shape()), pattern::wrap_type<opset1::Constant>()},
            pattern::has_static_shape()),
        matcher_name);
    register_matcher(m, convert_reduce_to_pooling<opset1::ReduceSum>());
}
