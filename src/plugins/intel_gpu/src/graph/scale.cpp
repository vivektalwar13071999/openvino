// Copyright (C) 2018-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "scale_inst.h"
#include "primitive_type_base.h"
#include "intel_gpu/runtime/error_handler.hpp"
#include "json_object.h"
#include <string>

namespace cldnn {
primitive_type_id scale::type_id() {
    static primitive_type_base<scale> instance;
    return &instance;
}

layout scale_inst::calc_output_layout(scale_node const& node) {
    auto desc = node.get_primitive();
    auto result = node.input().get_non_padded_output_layout();
    auto scale_layout = node.scale_in().get_non_padded_output_layout();

    auto scale_x_size = scale_layout.spatial(0);
    auto scale_y_size = scale_layout.spatial(1);
    auto scale_z_size = scale_layout.spatial(2);

    auto input_x_size = result.spatial(0);
    auto input_y_size = result.spatial(1);
    auto input_z_size = result.spatial(2);

    if ((result.data_type == data_types::u8 || result.data_type == data_types::i8 || result.data_type == data_types::i32) &&
        (node.scale_in().get_non_padded_output_layout().data_type == data_types::f32 ||
         node.scale_in().get_non_padded_output_layout().data_type == data_types::f16))
        result.data_type = node.scale_in().get_non_padded_output_layout().data_type;

    if (desc->output_data_type)
        result.data_type = *desc->output_data_type;

    if (node.has_fused_primitives()) {
        result.data_type = node.get_fused_output_layout().data_type;
    }

    if (scale_x_size != 1) {
        CLDNN_ERROR_NOT_EQUAL(node.id(), "Scale x size", scale_x_size, "input x size", input_x_size, "");
    }
    if (scale_y_size != 1) {
        CLDNN_ERROR_NOT_EQUAL(node.id(), "Scale y size", scale_y_size, "input y size", input_y_size, "");
    }
    if (scale_z_size != 1) {
        CLDNN_ERROR_NOT_EQUAL(node.id(), "Scale z size", scale_z_size, "input z size", input_z_size, "");
    }

    return result;
}

std::string scale_inst::to_string(scale_node const& node) {
    auto desc = node.get_primitive();
    auto node_info = node.desc_to_json();
    auto& input = node.input();
    auto& scale_input = node.scale_in();

    std::stringstream primitive_description;

    json_composite scale_info;
    scale_info.add("input", input.id());
    scale_info.add("scale input", scale_input.id());

    node_info->add("scale info", scale_info);
    node_info->dump(primitive_description);

    return primitive_description.str();
}

scale_inst::typed_primitive_inst(network& network, scale_node const& node) : parent(network, node) {
    auto scale_layout = node.scale_in().get_output_layout();
    auto scale_format = scale_layout.format;

    auto scale_batch_size = scale_layout.batch();
    auto scale_feature_size = scale_layout.feature();

    auto input_batch_size = node.input().get_output_layout().batch();
    auto input_feature_size = node.input().get_output_layout().feature();

    if (scale_batch_size != 1) {
        CLDNN_ERROR_NOT_EQUAL(node.id(),
                              "Scale batch size",
                              scale_batch_size,
                              "input batch size",
                              input_batch_size,
                              "");
    }

    if (scale_feature_size != 1) {
        CLDNN_ERROR_NOT_EQUAL(node.id(),
                              "Scale feature size",
                              scale_feature_size,
                              "input feature size",
                              input_feature_size,
                              "");
    }

    if (!argument.bias.empty()) {
        auto bias_layout = node.bias().get_output_layout();
        auto bias_format = bias_layout.format;
        auto bias_raw_sizes = bias_layout.size.raw;

        CLDNN_ERROR_NOT_PROPER_FORMAT(node.id(), "Scale format", scale_format.value, "bias format", bias_format);

        for (size_t i = 0; i < bias_layout.size.raw.size(); ++i) {
            if (scale_layout.size.raw[i] != bias_raw_sizes[i])
                CLDNN_ERROR_MESSAGE(node.id(),
                                    "Scale input size do not match bias size! Size index:" + std::to_string(i));
        }
    }
}
}  // namespace cldnn
