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
#include <remi/remi-client.h>

namespace py11 = pybind11;

typedef py11::capsule pymargo_instance_id;
typedef py11::capsule pymargo_addr;
typedef py11::capsule pyremi_client_t;
typedef py11::capsule pyremi_provider_handle_t;
typedef py11::capsule pyremi_fileset_t;

#define MID2CAPSULE(__mid)    py11::capsule((void*)(__mid),  "margo_instance_id")
#define ADDR2CAPSULE(__addr)  py11::capsule((void*)(__addr), "hg_addr_t")
#define REMIPH2CAPSULE(__rph) py11::capsule((void*)(__rph),  "remi_provider_handle_t")
#define REMICL2CAPSULE(__rcl) py11::capsule((void*)(__rcl),  "remi_client_t")
#define REMIFS2CAPSULE(__rfs) py11::capsule((void*)(__rfs),  "remi_fileset_t")

static pyremi_client_t pyremi_client_init(pymargo_instance_id mid) {
    remi_client_t result = REMI_CLIENT_NULL;
    remi_client_init(mid, ABT_IO_INSTANCE_NULL, &result);
    return REMICL2CAPSULE(result);
}

static pyremi_provider_handle_t pyremi_provider_handle_create(
        pyremi_client_t client,
        pymargo_addr addr,
        uint8_t provider_id) {

    remi_provider_handle_t providerHandle = REMI_PROVIDER_HANDLE_NULL;
    remi_provider_handle_create(client, addr, provider_id, &providerHandle);
    return REMIPH2CAPSULE(providerHandle);
}

static py11::object pyremi_migrate(
        pyremi_provider_handle_t source_ph,
        pyremi_fileset_t fileset,
        const std::string& dest_root,
        bool remove_source) {
    int ret;
    int status = 0;
    Py_BEGIN_ALLOW_THREADS
    int flag = remove_source ? REMI_REMOVE_SOURCE : REMI_KEEP_SOURCE;
    ret = remi_fileset_migrate(source_ph, fileset, dest_root.c_str(), flag, REMI_USE_MMAP, &status);
    Py_END_ALLOW_THREADS
    if(ret != REMI_SUCCESS) return py11::cast(false);
    return py11::cast(status);
}


PYBIND11_MODULE(_pyremiclient, m)
{
    py11::module::import("_pyremifileset");
    m.def("client_init", &pyremi_client_init);
    m.def("client_finalize", [](pyremi_client_t clt) {
            return remi_client_finalize(clt);} );
    m.def("provider_handle_create", &pyremi_provider_handle_create);
    m.def("provider_handle_ref_incr", [](pyremi_provider_handle_t prph) {
            return remi_provider_handle_ref_incr(prph); });
    m.def("provider_handle_release", [](pyremi_provider_handle_t prph) {
            return remi_provider_handle_release(prph); });
    m.def("migrate", &pyremi_migrate);
    m.def("shutdown_service", [](pyremi_client_t client, pymargo_addr addr) {
            return remi_shutdown_service(client, addr); });
}
