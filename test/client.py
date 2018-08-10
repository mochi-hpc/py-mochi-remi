# (C) 2018 The University of Chicago
# See COPYRIGHT in top-level directory.
import sys
sys.path.append('.')
sys.path.append('build/lib.linux-x86_64-2.7')
import os
from pymargo import MargoInstance
from pyremi.client import *
from pyremi.fileset import *

mid = MargoInstance('tcp')

server_addr = sys.argv[1]
provider_id = int(sys.argv[2])

client = RemiClient(mid)
addr = mid.lookup(server_addr)
ph = client.create_provider_handle(addr, provider_id)

cwd = os.getcwd()+"/"
# create a file
with open("myfile1.txt","w+") as f:
    f.write("This is an example file")
# and another one
with open("myfile2.txt","w+") as f:
    f.write("This is another example file")

fs = RemiFileset(migration_class="my_migration_class", root=cwd)
fs.register_file("myfile1.txt")
fs.register_file("myfile2.txt")
fs.register_metadata("matthieu","dorier")
fs.register_metadata("phil","carns")

def print_file(filename):
    print filename

def print_md(key, value):
    print key+" ==> "+value

print "Files in the fileset:"
fs.foreach_file(print_file)

print "Metadata in the fileset:"
fs.foreach_metadata(print_md)

ph.migrate(fs, cwd+"dest/", remove_source=False)
    
del ph
client.shutdown_service(addr)
del addr
client.finalize()
mid.finalize()
