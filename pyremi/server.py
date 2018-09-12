# (C) 2018 The University of Chicago
# See COPYRIGHT in top-level directory.
import _pyremiserver
from fileset import RemiFileset
import pymargo

class FilesetConvertor():

    def __init__(self, callback):
        self._callback = callback

    def __call__(self, fs):
        self._callback(RemiFileset(_handle=fs, _owner=False))

class RemiProvider(pymargo.Provider):

    def __init__(self, mid, provider_id):
        super(RemiProvider, self).__init__(mid, provider_id)
        self._provider = _pyremiserver.register(mid._mid, provider_id)

    def register_migration_class(self, class_name, pre_migration_callback, post_migration_callback):
        pre_cb = FilesetConvertor(pre_migration_callback)
        post_cb = FilesetConvertor(post_migration_callback)
        return _pyremiserver.register_migration_class(self._provider, str(class_name), pre_cb, post_cb)
