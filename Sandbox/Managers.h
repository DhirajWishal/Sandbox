#pragma once

typedef __int32 int;
typedef unsigned __int32 Uint;

typedef __int64 __int64;
typedef unsigned __int64 size_t;

///////////////////////////////////////////////////////////////////////////////////////////////////
////	Base Factories
///////////////////////////////////////////////////////////////////////////////////////////////////

class Buffer {};	// Set, Get, Size, Bind
class VertexBuffer : public Buffer {};	// Stride
class IndexBuffer : public Buffer {};	// Stride

class BufferFactory {
public:
	BufferFactory() {}
	virtual ~BufferFactory() {}

	virtual Buffer* CreateRawBuffer(size_t size) { return nullptr; }

	virtual IndexBuffer* CreateIndexBuffer(size_t stride, size_t vertexCount) { return nullptr; }
	virtual VertexBuffer* CreateVertexBuffer(size_t stride, size_t vertexCount) { return nullptr; }

	virtual void DestroyBuffer(Buffer* pBuffer) {}
};

class RenderTarget {};

class PipelineObject {};	// Bind
class GraphicsPipelineObject : public PipelineObject {};
class ComputePipelineObject : public PipelineObject {};
class RayTracingPipelineObject : public PipelineObject {};

class PipelineFactory {
public:
	PipelineFactory() {}
	virtual ~PipelineFactory() {}

	virtual GraphicsPipelineObject* CreateGraphicsPipeline() { return nullptr; }
	virtual ComputePipelineObject* CreateComputePipeline() { return nullptr; }
	virtual RayTracingPipelineObject* CreateRayTracingPipeline() { return nullptr; }
};

class CommandBufferInheritance {};

class CommandBuffer {
public:
	CommandBuffer() {}
	virtual ~CommandBuffer() {}

	virtual void BindInheritance(CommandBufferInheritance* pInheritance) {}

	virtual void BeginRecording() {}
	virtual void StopRecording() {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
////	Vulkan Implementation
///////////////////////////////////////////////////////////////////////////////////////////////////

class VulkanObject {
public:
	VulkanObject() {}
	virtual ~VulkanObject() {}

	void SetCore(/* Physical Device, Logical Device */) {}

	// Logical Device
	// Physical Device
};

class VulkanBuffer : public VulkanObject {};	// Memory handle
class VulkanVertexBuffer : public VertexBuffer, public VulkanBuffer {};
class VulkanIndexBuffer : public IndexBuffer, public VulkanBuffer {};

class VulkanBufferFactory : public BufferFactory, public VulkanObject {
public:
	VulkanBufferFactory() {}
	~VulkanBufferFactory() {}

	virtual Buffer* CreateRawBuffer(size_t size) override final { return nullptr; }

	virtual IndexBuffer* CreateIndexBuffer(size_t stride, size_t vertexCount) override final { return nullptr; }
	virtual VertexBuffer* CreateVertexBuffer(size_t stride, size_t vertexCount) override final { return nullptr; }

	virtual void DestroyBuffer(Buffer* pBuffer) override final {}
};

class VulkanPipelineFactory : public PipelineFactory, public VulkanObject {
public:
	VulkanPipelineFactory() {}
	~VulkanPipelineFactory() {}

	virtual GraphicsPipelineObject* CreateGraphicsPipeline() override final { return nullptr; }
	virtual ComputePipelineObject* CreateComputePipeline() override final { return nullptr; }
	virtual RayTracingPipelineObject* CreateRayTracingPipeline() override final { return nullptr; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
////	Graphcis Engine
///////////////////////////////////////////////////////////////////////////////////////////////////

class PrimitiveBuffer {
	struct Stride {};

public:
	PrimitiveBuffer() = default;
	/**
	 * Load data to the buffer and then push it to the GPU.
	 */
	PrimitiveBuffer(void* pData, Stride stride = {}, size_t size = 0)
		: pData(pData), stride(stride), size(size) {}

	/**
	 * Stream data directly to the GPU.
	 */
	PrimitiveBuffer(const char* pData, Stride stride = {})
		: pData((void*)pData), stride(stride), size(0) {}
	~PrimitiveBuffer() {}

	Stride stride;

	void* pData = nullptr;
	size_t size = 0;
};

class Material {};
class Texture : public Material {};

namespace MaterialFactory
{
	Material* CreateMetalic(const char* pName) { return nullptr; }
	Material* CreateTexture(const char* pAsset) { return nullptr; }
}

class MeshHandle {
public:
	MeshHandle() {}
	virtual ~MeshHandle() {}

	virtual void OnInitialize() {}
	virtual void OnUpdate(float timeStep) {}
	virtual void OnTerminate() {}

	virtual PrimitiveBuffer GetVertexBuffer() { return PrimitiveBuffer(); }
	virtual PrimitiveBuffer GetIndexBuffer() { return PrimitiveBuffer(); }

	void AddMaterial(Material* pMaterial) {}
};

/**
 * Engine.
 */
class GraphcisEngine {
public:
	GraphcisEngine() {}
	~GraphcisEngine() {}

	void Initialize()
	{
		pBufferFactory = new VulkanBufferFactory();
		pPipelineFactory = new VulkanPipelineFactory();
	}

	void Terminate()
	{
		delete pBufferFactory;
		delete pPipelineFactory;
	}

	BufferFactory* pBufferFactory = nullptr;
	PipelineFactory* pPipelineFactory = nullptr;
};
