# (C) 2018 The University of Chicago
# See COPYRIGHT in top-level directory.
import sys
sys.path.append('.')
sys.path.append('build/lib.linux-x86_64-2.7')
from pymargo import MargoInstance
from pyremi.server import RemiProvider
from pyremi.fileset import RemiFileset

mid = MargoInstance('tcp')
mid.enable_remote_shutdown()
provider_id = 42

def my_migration_callback(fileset):
    print "Migration of a new fileset completed"
    print "Fileset class is "+fileset.get_class()
    print "Fileset root is "+fileset.get_root()
    print "This fileset contains the following files:"
    for filename in fileset.get_files():
        print "--- " + str(filename)
    print "And the following metadata:"
    metadata = fileset.get_metadata()
    for key in metadata:
        print "--- " + str(key) + " ==> " + str(metadata[key])
    print "===="

provider = RemiProvider(mid, provider_id)
provider.register_migration_class("my_migration_class", my_migration_callback)

print "Server running at address "+str(mid.addr())+" with provider_id="+str(provider_id)

mid.wait_for_finalize()
