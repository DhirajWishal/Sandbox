typedef void* GraphicsBackendDataPointer;

template<class Type>
GraphicsBackendDataPointer CreateBackendPointer()
{
	return new Type();
}

struct MeshHandle {};

namespace Vulkan
{
	struct VulkanBackendCore {};

	struct VulkanMeshHandle { MeshHandle* pParentHandle = nullptr; };

	void InitializeMeshHandle(MeshHandle* pMeshHandle)
	{

	}
}