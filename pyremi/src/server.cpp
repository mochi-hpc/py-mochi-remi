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
#include <remi/remi-server.h>

namespace py11 = pybind11;

typedef py11::capsule pymargo_instance_id;
typedef py11::capsule pyremi_provider_t;
typedef py11::capsule pyremi_fileset_t;

#define MID2CAPSULE(__mid)    py11::capsule((void*)(__mid),  "margo_instance_id", nullptr)
#define ADDR2CAPSULE(__addr)  py11::capsule((void*)(__addr), "hg_addr_t", nullptr)
#define REMIPR2CAPSULE(__rpr) py11::capsule((void*)(__rpr),  "remi_provider_t", nullptr)
#define REMIFS2CAPSULE(__rfs) py11::capsule((void*)(__rfs),  "remi_fileset_t", nullptr)

static pyremi_provider_t pyremi_provider_register(pymargo_instance_id mid, uint8_t provider_id) {
    remi_provider_t provider;
    int ret = remi_provider_register(mid, provider_id, REMI_ABT_POOL_DEFAULT, &provider);
    if(ret != 0) return py11::none();
    else return REMIPR2CAPSULE(provider);
}

static int pre_migration_callback(remi_fileset_t fileset, void* uargs) {
    auto fun = static_cast<std::pair<py11::object,py11::object>*>(uargs);
    int ret = 0;
    PyGILState_STATE gstate = PyGILState_Ensure();
    try {
        auto theFileset = REMIFS2CAPSULE(fileset);
        ret = py11::cast<int>((fun->first)(theFileset));
    } catch(const py11::error_already_set&) {
        PyErr_Print();
        ret = -1;
    }
    PyGILState_Release(gstate);
    return ret;
}

static int post_migration_callback(remi_fileset_t fileset, void* uargs) {
    auto fun = static_cast<std::pair<py11::object,py11::object>*>(uargs);
    int ret = 0;
    PyGILState_STATE gstate = PyGILState_Ensure();
    try {
        auto theFileset = REMIFS2CAPSULE(fileset);
        ret = py11::cast<int>((fun->second)(theFileset));
    } catch(const py11::error_already_set&) {
        PyErr_Print();
        ret = -1;
    }
    PyGILState_Release(gstate);
    return ret;
}

static void free_callback(void* uargs) {
    auto cbs = static_cast<std::pair<py11::object,py11::object>*>(uargs);
    delete cbs;
}

static int pyremi_provider_register_migration_class(
        pyremi_provider_t provider,
        const std::string& class_name,
        py11::object& pre_cb,
        py11::object& post_cb) 
{
    auto cbs = new std::pair<py11::object,py11::object>(pre_cb, post_cb);
    return remi_provider_register_migration_class(provider,
            class_name.c_str(), pre_migration_callback, 
            post_migration_callback, free_callback, static_cast<void*>(cbs));
}

PYBIND11_MODULE(_pyremiserver, m)
{
    py11::module::import("_pyremifileset");
    m.def("register", &pyremi_provider_register);
    m.def("register_migration_class", &pyremi_provider_register_migration_class);
}
