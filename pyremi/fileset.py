# (C) 2018 The University of Chicago
# See COPYRIGHT in top-level directory.
import _pyremifileset
import pymargo

class InvalidArgumentError(ValueError):
    pass

class FilesCollector():

    def __init__(self):
        self._files = []

    def __call__(self, filename):
        self._files.append(filename)

class MetadataCollector():

    def __init__(self):
        self._metadata = dict()

    def __call__(self, key, value):
        self._metadata[key] = value

class RemiFileset():

    def __init__(self, **kwargs):
        fileset_class = kwargs.get('migration_class',None)
        fileset_root  = kwargs.get('root',None)
        handle        = kwargs.get('_handle',None)
        owner         = kwargs.get('_owner',False)
        if(not (handle is None)):
            self._fs = handle
            self._owner = owner
        elif(not (fileset_class is None or fileset_root is None)):
            self._fs = _pyremifileset.fileset_create(str(fileset_class), str(fileset_root))
            self._owner = True
        else:
            raise InvalidArgumentError()

    def __del__(self):
        if(self._owner):
            _pyremifileset.fileset_free(self._fs)

    def get_class(self):
        return _pyremifileset.get_class(self._fs)

    def get_root(self):
        return _pyremifileset.get_root(self._fs)

    def register_file(self, filename):
        return _pyremifileset.register_file(self._fs, str(filename))

    def deregister_file(self, filename):
        return _pyremifileset.deregister_file(self._fs, str(filename))

    def register_metadata(self, key, value):
        return _pyremifileset.register_metadata(self._fs, str(key), str(value))

    def deregister_metadata(self, key):
        return _pyremifileset.deregister_metadata(self._fs, str(key))

    def foreach_file(self, callback):
        return _pyremifileset.foreach_file(self._fs, callback)

    def foreach_metadata(self, callback):
        return _pyremifileset.foreach_metadata(self._fs, callback)

    def get_files(self):
        collector = FilesCollector()
        self.foreach_file(collector)
        return collector._files

    def get_metadata(self):
        collector = MetadataCollector()
        self.foreach_metadata(collector)
        return collector._metadata

