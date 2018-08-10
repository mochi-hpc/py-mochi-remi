/*
 * (C) 2018 The University of Chicago
 * 
 * See COPYRIGHT in top-level directory.
 */
#include <pybind11/pybind11.h>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include <margo.h>
#include <remi/remi-client.h>

namespace py11 = pybind11;

typedef py11::capsule pyremi_fileset_t;
#define REMIFS2CAPSULE(__rfs) py11::capsule((void*)(__rfs), "remi_fileset_t", nullptr)

static void fileset_files_cb(const char* filename, void* uargs) {
    if(uargs == nullptr) return;
    py11::object* fun = static_cast<py11::object*>(uargs);
    PyGILState_STATE gstate = PyGILState_Ensure();
    try {
        std::string theFilename(filename);
        (*fun)(theFilename);
    } catch(const py11::error_already_set&) {
        PyErr_Print();
    }
    PyGILState_Release(gstate);
}

static void fileset_metadata_cb(const char* key, const char* value, void* uargs) {
    if(uargs == nullptr) return;
    py11::object* fun = static_cast<py11::object*>(uargs);
    PyGILState_STATE gstate = PyGILState_Ensure();
    try {
        std::string theKey(key);
        std::string theValue(value);
        (*fun)(theKey, theValue);
    } catch(const py11::error_already_set&) {
        PyErr_Print();
    }
    PyGILState_Release(gstate);
}

PYBIND11_MODULE(_pyremifileset, m)
{
    m.def("fileset_create", [](const std::string& fileset_class, const std::string& fileset_root) -> py11::object {
        remi_fileset_t fileset = REMI_FILESET_NULL;
        int ret = remi_fileset_create(fileset_class.c_str(), fileset_root.c_str(), &fileset);
        if(ret != REMI_SUCCESS) return py11::none();
        else return REMIFS2CAPSULE(fileset);
    });

    m.def("fileset_free", [](pyremi_fileset_t prfs) {
        return remi_fileset_free(prfs);
    });

    m.def("get_class", [](pyremi_fileset_t prfs) -> py11::object {
        std::vector<char> buf;
        size_t s = 0;
        int ret = remi_fileset_get_class(prfs, nullptr, &s);
        if(ret != HG_SUCCESS) return py11::none();
        buf.resize(s);
        ret = remi_fileset_get_class(prfs, buf.data(), &s);
        if(ret != HG_SUCCESS) return py11::none();
        std::string klass(buf.data());
        return py11::cast(klass);
    });

    m.def("get_root", [](pyremi_fileset_t prfs) -> py11::object {
        std::vector<char> buf;
        size_t s = 0;
        int ret = remi_fileset_get_root(prfs, nullptr, &s);
        if(ret != HG_SUCCESS) return py11::none();
        buf.resize(s);
        ret = remi_fileset_get_root(prfs, buf.data(), &s);
        if(ret != HG_SUCCESS) return py11::none();
        std::string root(buf.data());
        return py11::cast(root);
    });

    m.def("register_file", [](pyremi_fileset_t prfs, const std::string& filename) {
        return remi_fileset_register_file(prfs, filename.c_str());
    });

    m.def("deregister_file", [](pyremi_fileset_t prfs, const std::string& filename) {
        return remi_fileset_deregister_file(prfs, filename.c_str());
    });

    m.def("foreach_file", [](pyremi_fileset_t prfs, py11::object& fun) {
        return remi_fileset_foreach_file(prfs, fileset_files_cb, static_cast<void*>(&fun));
    });

    m.def("register_metadata", [](pyremi_fileset_t prfs, const std::string& key, const std::string& val) {
        return remi_fileset_register_metadata(prfs, key.c_str(), val.c_str());
    });

    m.def("deregister_metadata", [](pyremi_fileset_t prfs, const std::string& key) {
        return remi_fileset_deregister_metadata(prfs, key.c_str());
    });

    m.def("foreach_metadata", [](pyremi_fileset_t prfs, py11::object& fun) {
        return remi_fileset_foreach_metadata(prfs, fileset_metadata_cb, static_cast<void*>(&fun));
    });
}
