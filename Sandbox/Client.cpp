#include "Compute.h"

int main()
{
	Handle pInstance = create_compute_instance();
	Handle pDevice = create_compute_device(pInstance);
	Handle pBuffer = create_compute_storage_buffer(pDevice, 1024);
	Handle pImage = create_compute_storage_image(pDevice, 1280, 720, 1);

	void* pMemoryRange = map_memory_from_buffer(pDevice, pBuffer, 1024, 0);
	unmap_memory_from_buffer(pDevice, pBuffer);

	destroy_compute_storage_image(pDevice, pImage);
	destroy_compute_storage_buffer(pDevice, pBuffer);
	destroy_compute_device(pDevice);
	destroy_compute_instance(pInstance);
}