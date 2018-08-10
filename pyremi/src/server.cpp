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

static void migration_callback(remi_fileset_t fileset, void* uargs) {
    py11::object* fun = static_cast<py11::object*>(uargs);
    PyGILState_STATE gstate = PyGILState_Ensure();
    try {
        auto theFileset = REMIFS2CAPSULE(fileset);
        (*fun)(theFileset);
    } catch(const py11::error_already_set&) {
        PyErr_Print();
    }
    PyGILState_Release(gstate);
}

static void free_callback(void* uargs) {
    py11::object* fun = static_cast<py11::object*>(uargs);
    delete fun;
}

static int pyremi_provider_register_migration_class(
        pyremi_provider_t provider,
        const std::string& class_name,
        py11::object& cb) 
{
    auto cb2 = new py11::object(cb);
    return remi_provider_register_migration_class(provider,
            class_name.c_str(), migration_callback, free_callback, static_cast<void*>(cb2));
}

PYBIND11_MODULE(_pyremiserver, m)
{
    py11::module::import("_pyremifileset");
    m.def("register", &pyremi_provider_register);
    m.def("register_migration_class", &pyremi_provider_register_migration_class);
}
