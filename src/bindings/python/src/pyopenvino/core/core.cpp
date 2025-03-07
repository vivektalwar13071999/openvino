// Copyright (C) 2018-2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "pyopenvino/core/core.hpp"

#include <ie_extension.h>
#include <pybind11/stl.h>

#include <openvino/core/any.hpp>
#include <openvino/runtime/core.hpp>
#include <pyopenvino/core/tensor.hpp>

#include "common.hpp"
#include "pyopenvino/utils/utils.hpp"

namespace py = pybind11;

std::string to_string(py::handle handle) {
    auto encodedString = PyUnicode_AsUTF8String(handle.ptr());
    return PyBytes_AsString(encodedString);
}

void regclass_Core(py::module m) {
    py::class_<ov::Core, std::shared_ptr<ov::Core>> cls(m, "Core");
    cls.doc() =
        "openvino.runtime.Core class represents OpenVINO runtime Core entity. User applications can create several "
        "Core class instances, but in this case, the underlying plugins are created multiple times and not shared "
        "between several Core instances. The recommended way is to have a single Core instance per application.";

    cls.def(py::init<const std::string&>(), py::arg("xml_config_file") = "");

    cls.def(
        "set_property",
        [](ov::Core& self, const std::map<std::string, py::object>& properties) {
            self.set_property(Common::utils::properties_to_any_map(properties));
        },
        py::arg("properties"),
        R"(
            Sets properties.

            :param properties: Dict of pairs: (property name, property value).
            :type properties: dict
        )");

    // Overload for single tuple
    cls.def(
        "set_property",
        [](ov::Core& self, const std::pair<std::string, py::object>& property) {
            ov::AnyMap _properties{{property.first, py_object_to_any(property.second)}};
            self.set_property(_properties);
        },
        py::arg("property"),
        R"(
            Sets properties for the device.

            :param property: Tuple of (property name, matching property value).
            :type property: tuple
        )");

    cls.def(
        "set_property",
        [](ov::Core& self, const std::string& device_name, const std::map<std::string, py::object>& properties) {
            self.set_property(device_name, Common::utils::properties_to_any_map(properties));
        },
        py::arg("device_name"),
        py::arg("properties"),
        R"(
            Sets properties for the device.

            :param device_name: Name of the device.
            :type device_name: str
            :param properties: Dict of pairs: (property name, property value).
            :type properties: dict
        )");

    // Overload for single tuple
    cls.def(
        "set_property",
        [](ov::Core& self, const std::string& device_name, const std::pair<std::string, py::object>& property) {
            ov::AnyMap _properties{{property.first, py_object_to_any(property.second)}};
            self.set_property(device_name, _properties);
        },
        py::arg("device_name"),
        py::arg("property"),
        R"(
            Sets properties for the device.

            :param device_name: Name of the device.
            :type device_name: str
            :param property: Tuple of (property name, matching property value).
            :type property: tuple
        )");

    cls.def(
        "get_property",
        [](ov::Core& self, const std::string& device_name, const std::string& property) -> py::object {
            return Common::utils::from_ov_any(self.get_property(device_name, property));
        },
        py::arg("device_name"),
        py::arg("property"),
        R"(
            Gets properties dedicated to device behaviour.

            :param device_name: A name of a device to get a properties value.
            :type device_name: str
            :param property: Property or name of Property.
            :type property: str
            :return: Extracted information from property.
            :rtype: object
        )");

    cls.def(
        "compile_model",
        [](ov::Core& self,
           const std::shared_ptr<const ov::Model>& model,
           const std::string& device_name,
           const std::map<std::string, py::object>& properties) {
            auto _properties = Common::utils::properties_to_any_map(properties);
            py::gil_scoped_release release;
            return self.compile_model(model, device_name, _properties);
        },
        py::arg("model"),
        py::arg("device_name"),
        py::arg("properties"),
        R"(
            Creates a compiled model from a source model object.
            Users can create as many compiled models as they need, and use them simultaneously
            (up to the limitation of the hardware resources).

            GIL is released while running this function.

            :param model: Model acquired from read_model function.
            :type model: openvino.runtime.Model
            :param device_name: Name of the device which will load the model.
            :type device_name: str
            :param properties: Optional dict of pairs: (property name, property value) relevant only for this load operation.
            :type properties: dict
            :return: A compiled model.
            :rtype: openvino.runtime.CompiledModel
        )");

    cls.def(
        "compile_model",
        [](ov::Core& self,
           const std::shared_ptr<const ov::Model>& model,
           const std::map<std::string, py::object>& properties) {
            auto _properties = Common::utils::properties_to_any_map(properties);
            py::gil_scoped_release release;
            return self.compile_model(model, _properties);
        },
        py::arg("model"),
        py::arg("properties"),
        R"(
            Creates and loads a compiled model from a source model to the default OpenVINO device
            selected by AUTO plugin. Users can create as many compiled models as they need, and use
            them simultaneously (up to the limitation of the hardware resources).

            GIL is released while running this function.

            :param model: Model acquired from read_model function.
            :type model: openvino.runtime.Model
            :param properties: Optional dict of pairs: (property name, property value) relevant only for this load operation.
            :type properties: dict
            :return: A compiled model.
            :rtype: openvino.runtime.CompiledModel
        )");

    cls.def(
        "compile_model",
        [](ov::Core& self,
           const std::string& model_path,
           const std::string& device_name,
           const std::map<std::string, py::object>& properties) {
            auto _properties = Common::utils::properties_to_any_map(properties);
            py::gil_scoped_release release;
            return self.compile_model(model_path, device_name, _properties);
        },
        py::arg("model_path"),
        py::arg("device_name"),
        py::arg("properties"),
        R"(
            Reads model and creates a compiled model from IR / ONNX / PDPD file.
            This can be more efficient than using read_model + compile_model(model_in_memory_object) flow,
            especially for cases when caching is enabled and cached model is available.

            GIL is released while running this function.

            :param model_path: A path to a model in IR / ONNX / PDPD format.
            :type model_path: str
            :param device_name: Name of the device to load the model to.
            :type device_name: str
            :param properties: Optional dict of pairs: (property name, property value) relevant only for this load operation.
            :type properties: dict
            :return: A compiled model.
            :rtype: openvino.runtime.CompiledModel
        )");

    cls.def(
        "compile_model",
        [](ov::Core& self, const std::string& model_path, const std::map<std::string, py::object>& properties) {
            auto _properties = Common::utils::properties_to_any_map(properties);
            py::gil_scoped_release release;
            return self.compile_model(model_path, _properties);
        },
        py::arg("model_path"),
        py::arg("properties"),
        R"(
            Reads model and creates a compiled model from IR / ONNX / PDPD file with device selected by AUTO plugin.
            This can be more efficient than using read_model + compile_model(model_in_memory_object) flow,
            especially for cases when caching is enabled and cached model is available.

            GIL is released while running this function.

            :param model_path: A path to a model in IR / ONNX / PDPD format.
            :type model_path: str
            :param properties: Optional dict of pairs: (property name, property value) relevant only for this load operation.
            :type properties: dict
            :return: A compiled model.
            :rtype: openvino.runtime.CompiledModel
        )");

    cls.def("get_versions",
            &ov::Core::get_versions,
            py::arg("device_name"),
            R"(
                Returns device plugins version information.

                :param device_name: Device name to identify a plugin.
                :type device_name: str
                :return: Plugin version information.
                :rtype: Dict[str, openvino.runtime.Version]
            )");

    cls.def(
        "read_model",
        [](ov::Core& self, py::bytes model, py::bytes weights) {
            std::string ir(model);
            // works on view in order to omit copying bytes into string
            py::buffer_info info(py::buffer(weights).request());
            size_t bin_size = static_cast<size_t>(info.size);
            ov::Tensor tensor(ov::element::Type_t::u8, {bin_size});
            // if weights are not empty
            if (bin_size) {
                const uint8_t* bin = reinterpret_cast<const uint8_t*>(info.ptr);
                std::memcpy(tensor.data(), bin, bin_size);
            }
            py::gil_scoped_release release;
            return self.read_model(ir, tensor);
        },
        py::arg("model"),
        py::arg("weights") = py::bytes(),
        R"(
            Reads models from IR / ONNX / PDPD formats.

            GIL is released while running this function.

            :param model: Bytes with model in IR / ONNX / PDPD format.
            :type model: bytes
            :param weights: Bytes with tensor's data.
            :type weights: bytes
            :return: A model.
            :rtype: openvino.runtime.Model
        )");

    cls.def(
        "read_model",
        (std::shared_ptr<ov::Model>(ov::Core::*)(const std::string&, const std::string&) const) & ov::Core::read_model,
        py::call_guard<py::gil_scoped_release>(),
        py::arg("model"),
        py::arg("weights") = "",
        R"(
            Reads models from IR / ONNX / PDPD formats.

            GIL is released while running this function.

            :param model: A path to a model in IR / ONNX / PDPD format.
            :type model: str
            :param weights: A path to a data file For IR format (*.bin): if path is empty,
                            it tries to read a bin file with the same name as xml and if the bin
                            file with the same name was not found, loads IR without weights.
                            For ONNX format (*.onnx): weights parameter is not used.
                            For PDPD format (*.pdmodel) weights parameter is not used.
            :type weights: str
            :return: A model.
            :rtype: openvino.runtime.Model
        )");

    cls.def(
        "read_model",
        (std::shared_ptr<ov::Model>(ov::Core::*)(const std::string&, const ov::Tensor&) const) & ov::Core::read_model,
        py::call_guard<py::gil_scoped_release>(),
        py::arg("model"),
        py::arg("weights"),
        R"(
            Reads models from IR / ONNX / PDPD formats.

            GIL is released while running this function.

            :param model: A string with model in IR / ONNX / PDPD format.
            :type model: str
            :param weights: Tensor with weights. Reading ONNX / PDPD models doesn't support
                            loading weights from weights tensors.
            :type weights: openvino.runtime.Tensor
            :return: A model.
            :rtype: openvino.runtime.Model
        )");

    cls.def(
        "read_model",
        [](ov::Core& self, py::object model_path, py::object weights_path) {
            std::string model_path_cpp{py::str(model_path)};
            std::string weights_path_cpp{py::str(weights_path)};
            py::gil_scoped_release release;
            return self.read_model(model_path_cpp, weights_path_cpp);
        },
        py::arg("model"),
        py::arg("weights") = "",
        R"(
            Reads models from IR / ONNX / PDPD formats.

            GIL is released while running this function.

            :param model: A string with model in IR / ONNX / PDPD format.
            :type model: str
            :param weights: A path to a data file For IR format (*.bin): if path is empty,
                            it tries to read a bin file with the same name as xml and if the bin
                            file with the same name was not found, loads IR without weights.
                            For ONNX format (*.onnx): weights parameter is not used.
                            For PDPD format (*.pdmodel) weights parameter is not used.
            :type weights: str
            :return: A model.
            :rtype: openvino.runtime.Model
        )");

    cls.def(
        "import_model",
        [](ov::Core& self,
           const std::string& model_stream,
           const std::string& device_name,
           const std::map<std::string, py::object>& properties) {
            auto _properties = Common::utils::properties_to_any_map(properties);
            py::gil_scoped_release release;
            std::stringstream _stream;
            _stream << model_stream;
            return self.import_model(_stream, device_name, _properties);
        },
        py::arg("model_stream"),
        py::arg("device_name"),
        py::arg("properties"),
        R"(
            Imports a compiled model from a previously exported one.

            GIL is released while running this function.

            :param model_stream: Input stream, containing a model previously exported, using export_model method.
            :type model_stream: bytes
            :param device_name: Name of device to which compiled model is imported.
                                Note: if device_name is not used to compile the original model, an exception is thrown.
            :type device_name: str
            :param properties: Optional map of pairs: (property name, property value) relevant only for this load operation.
            :type properties: dict, optional
            :return: A compiled model.
            :rtype: openvino.runtime.CompiledModel

            :Example:
            .. code-block:: python

                user_stream = compiled.export_model()

                with open('./my_model', 'wb') as f:
                    f.write(user_stream)

                # ...

                new_compiled = core.import_model(user_stream, "CPU")
        )");

    // keep as second one to solve overload resolution problem
    cls.def(
        "import_model",
        [](ov::Core& self,
           const py::object& model_stream,
           const std::string& device_name,
           const std::map<std::string, py::object>& properties) {
            auto _properties = Common::utils::properties_to_any_map(properties);
            if (!(py::isinstance(model_stream, pybind11::module::import("io").attr("BytesIO")))) {
                throw py::type_error("CompiledModel.import_model(model_stream) incompatible function argument: "
                                     "`model_stream` must be an io.BytesIO object but " +
                                     (std::string)(py::repr(model_stream)) + "` provided");
            }
            model_stream.attr("seek")(0);  // Always rewind stream!
            std::stringstream _stream;
            _stream << model_stream
                           .attr("read")()  // alternative: model_stream.attr("get_value")()
                           .cast<std::string>();
            py::gil_scoped_release release;
            return self.import_model(_stream, device_name, _properties);
        },
        py::arg("model_stream"),
        py::arg("device_name"),
        py::arg("properties"),
        R"(
            Imports a compiled model from a previously exported one.

            Advanced version of `import_model`. It utilizes, streams from standard
            Python library `io`.

            GIL is released while running this function.


            :param model_stream: Input stream, containing a model previously exported, using export_model method.
            :type model_stream: io.BytesIO
            :param device_name: Name of device to which compiled model is imported.
                                Note: if device_name is not used to compile the original model, an exception is thrown.
            :type device_name: str
            :param properties: Optional map of pairs: (property name, property value) relevant only for this load operation.
            :type properties: dict, optional
            :return: A compiled model.
            :rtype: openvino.runtime.CompiledModel

            :Example:
            .. code-block:: python

                user_stream = io.BytesIO()
                compiled.export_model(user_stream)

                with open('./my_model', 'wb') as f:
                    f.write(user_stream.getvalue()) # or read() if seek(0) was applied before

                # ...

                new_compiled = core.import_model(user_stream, "CPU")
        )");

    cls.def("register_plugin",
            &ov::Core::register_plugin,
            py::arg("plugin_name"),
            py::arg("device_name"),
            R"(
                Register a new device and plugin which enable this device inside OpenVINO Runtime.

                :param plugin_name: A name of plugin. Depending on platform `plugin_name` is wrapped with shared library
                                    suffix and prefix to identify library full name E.g. on Linux platform plugin name
                                    specified as `plugin_name` will be wrapped as `libplugin_name.so`.
                :type plugin_name: str
                :param device_name: A device name to register plugin for.
                :type device_name: str
            )");

    cls.def("register_plugins",
            &ov::Core::register_plugins,
            py::arg("xml_config_file"),
            R"(
                Registers a device plugin to OpenVINO Runtime Core instance using XML configuration
                file with plugins description.

                :param xml_config_file: A path to .xml file with plugins to register.
                :type xml_config_file: str
            )");

    cls.def("unload_plugin",
            &ov::Core::unload_plugin,
            py::arg("device_name"),
            R"(
                Unloads the previously loaded plugin identified by device_name from OpenVINO Runtime.
                The method is needed to remove loaded plugin instance and free its resources.
                If plugin for a specified device has not been created before, the method throws an exception.

                :param device_name: A device name identifying plugin to remove from OpenVINO.
                :type device_name: str
            )");

    cls.def(
        "query_model",
        [](ov::Core& self,
           const std::shared_ptr<const ov::Model>& model,
           const std::string& device_name,
           const std::map<std::string, py::object>& properties) {
            auto _properties = Common::utils::properties_to_any_map(properties);
            py::gil_scoped_release release;
            return self.query_model(model, device_name, _properties);
        },
        py::arg("model"),
        py::arg("device_name"),
        py::arg("properties") = py::dict(),
        R"(
            Query device if it supports specified model with specified properties.

            GIL is released while running this function.

            :param model: Model object to query.
            :type model: openvino.runtime.Model
            :param device_name: A name of a device to query.
            :type device_name: str
            :param properties: Optional dict of pairs: (property name, property value)
            :type properties: dict
            :return: Pairs a operation name -> a device name supporting this operation.
            :rtype: dict                
        )");

    cls.def("add_extension",
            static_cast<void (ov::Core::*)(const std::string&)>(&ov::Core::add_extension),
            py::arg("library_path"),
            R"(
                Registers an extension to a Core object.

                :param library_path: Path to library with ov::Extension
                :type library_path: str
            )");

    cls.def("add_extension",
            static_cast<void (ov::Core::*)(const std::shared_ptr<ov::Extension>&)>(&ov::Core::add_extension),
            py::arg("extension"),
            R"(
                Registers an extension to a Core object.
                
                :param extension: Extension object.
                :type extension: openvino.runtime.Extension
            )");

    cls.def(
        "add_extension",
        static_cast<void (ov::Core::*)(const std::vector<std::shared_ptr<ov::Extension>>&)>(&ov::Core::add_extension),
        py::arg("extensions"),
        R"(
            Registers extensions to a Core object.

            :param extensions: List of Extension objects.
            :type extensions: list[openvino.runtime.Extension]
        )");

    cls.def_property_readonly("available_devices",
                              &ov::Core::get_available_devices,
                              py::call_guard<py::gil_scoped_release>(),
                              R"(
                                    Returns devices available for inference Core objects goes over all registered plugins.

                                    GIL is released while running this function.

                                    :returns: A list of devices. The devices are returned as: CPU, GPU.0, GPU.1, MYRIAD...
                                        If there more than one device of specific type, they are enumerated with .# suffix.
                                        Such enumerated device can later be used as a device name in all Core methods like:
                                        compile_model, query_model, set_property and so on.
                                    :rtype: list
                                )");
}
