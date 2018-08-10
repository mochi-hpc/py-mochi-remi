# (C) 2018 The University of Chicago
# See COPYRIGHT in top-level directory.
import _pyremiclient
from pyremi.fileset import *
import pymargo

class RemiClient():
    """
    The RemiClient class wraps a remi_client_t structure at C level.
    It registers the RPCs necessary to interact with a Remi provider.
    It can be used to create provider handles pointing to Remi providers.
    """

    def __init__(self, mid):
        """
        Constructor. Initializes a new RemiClient with a MargoInstance.

        Args:
            mid (MargoInstance): MargoInstance on which to register RPCs.
        """
        self._client = _pyremiclient.client_init(mid._mid)

    def create_provider_handle(self, addr, provider_id):
        """
        Creates a RemiProviderHandle object pointing to the given
        address and provider id.

        Args:
            addr (MargoAddress): Address of the Remi provider.
            provider_id (int): ID of the provider.
        """
        ph = _pyremiclient.provider_handle_create(self._client, addr.get_hg_addr(), provider_id)
        return RemiProviderHandle(ph)

    def shutdown_service(self, addr):
        """
        Shut down a MargoInstance running at a particular address.

        Args:
            addr (MargoAddress): Address of the MargoInstance to shut down.
        """
        _pyremiclient.shutdown_service(self._client, addr.get_hg_addr())
	
    def finalize(self):
        """
        Finalizes the underlying remi_client_t structure.
        """
        _pyremiclient.client_finalize(self._client)

class RemiProviderHandle():
    """
    The RemiProviderHandle class represents a handle to a remote Remi provider.
    Internally, this class wraps a remi_provider_handle_t C structure.
    """

    def __init__(self, ph):
        """
        Constructor. This is not supposed to be called by users.
        Users should create a RemiProviderHandle from a RemiClient clt
        by calling clt.create_provider_handle.
        """
        self._ph = ph
	
    def __del__(self):
        """
        Explicit destructor to call provider_handle_release on the underlying
        remi_provider_handle_t C structure.
        """
        _pyremiclient.provider_handle_release(self._ph)


    def migrate(self, fileset, dest_root, remove_source=True):
        """
        Migrates a local fileset to the provider pointed by this provider handle.
        This function will also remove the original fileset if remove_source 
        is set to True.

        Args:
            fileset (Fileset): Fileset to migrate.
            dest_root (str): destination root for the fileset.
            remove_source (bool): whether to remove the source files.
        Returns:
            The True if successful, False otherwise.
        """
        return _pyremiclient.migrate(self._ph, fileset._fs, str(dest_root), remove_source)
