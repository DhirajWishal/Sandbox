#pragma once
#include <vector>
#include <unordered_map>

class PrimitiveBuffer {
public:
	PrimitiveBuffer(size_t stride = 0, size_t count = 0, void* data = nullptr) : stride(stride), count(count), data(data) {}
	virtual ~PrimitiveBuffer() {}

	size_t getByteSize() const { return stride * count; }
	size_t getStride() const { return stride; }
	size_t getVertexCount() const { return count; }
	void* getData() const { return data; }

	void copyData(size_t stride, size_t count, void* pSource)
	{
		this->stride = stride;
		this->count = count;

		if (data)
			delete data;

		data = new short[count * stride];
		std::memcpy(data, pSource, count * stride);
	}

public:
	size_t stride = 0;
	size_t count = 0;
	void* data = nullptr;
};

using VertexBuffer = PrimitiveBuffer;
using IndexBuffer = PrimitiveBuffer;

class UniformBuffer {
	struct UniformAttribute {
		size_t size = 0;
		size_t offset = 0;
	};

public:
	UniformBuffer() {}
	~UniformBuffer() {}

	std::unordered_map<std::string, UniformAttribute> uniformAttributes;
};

class MeshHandle {
public:
	MeshHandle() {}
	virtual ~MeshHandle() {}

	/**
	 * Submit vertex data using this.
	 */
	virtual VertexBuffer getVertexBuffer() { return VertexBuffer(); }

	/**
	 * Submit index data using this.
	 */
	virtual IndexBuffer getIndexBuffer() { return IndexBuffer(); }

	/**
	 * Submit uniform buffers using this.
	 */
	virtual std::vector<UniformBuffer> getUniformBuffers() { return std::vector<UniformBuffer>(); }

	/**
	 * Empty the contents of the mesh handle. 
	 * This method is explicitly called after caching data in the GPU memory prior to rendering.
	 */
	virtual void clear() {}
};

//////////////////////////////////////////////////////////////////////////////////////////////////
////	Render Definitions
//////////////////////////////////////////////////////////////////////////////////////////////////

class GObject {
public:
	GObject() {}
	virtual ~GObject() {}

	template<class Type>
	__forceinline Type* inherit() { return dynamic_cast<Type*>(this); }
};

class GCommandBuffer : public GObject {};

class GBuffer : public GObject {
public:
	GBuffer() {}
	virtual ~GBuffer() {}

	virtual void bind(GCommandBuffer* pCommandBuffer) {}

	virtual void update(GCommandBuffer* pCommandBuffer = nullptr) {}
	virtual void* mapToLocal(size_t size, size_t offset) { return nullptr; }
	virtual void unmapFromLocal() {}
};

template<class Type = PrimitiveBuffer>
class GPrimitiveBuffer : public GBuffer {
public:
	using Primitive = Type;

public:
	GPrimitiveBuffer() {}
	virtual ~GPrimitiveBuffer() {}

	__forceinline void setParent(Type* pParent) { this->pParentObject = pParent; }
	__forceinline Type* getParent() const { return this->pParentObject; }

protected:
	Type* pParentObject = nullptr;
};

using GVertexBuffer = GPrimitiveBuffer<VertexBuffer>;
using GIndexBuffer = GPrimitiveBuffer<IndexBuffer>;
using GUniformBuffer = GPrimitiveBuffer<UniformBuffer>;

//////////////////////////////////////////////////////////////////////////////////////////////////
////	User Definition
//////////////////////////////////////////////////////////////////////////////////////////////////

class StaticMesh : public MeshHandle {
public:
	StaticMesh() {}
	~StaticMesh() {}

	/**
	 * Submit vertex data using this.
	 */
	virtual VertexBuffer getVertexBuffer() override final
	{
		return VertexBuffer(sizeof(float), vertexes.size(), vertexes.data());
	}

	/**
	 * Submit index data using this.
	 */
	virtual IndexBuffer getIndexBuffer() override final
	{
		return IndexBuffer(sizeof(int), indexes.size(), indexes.data());
	}

	/**
	 * Empty the contents of the mesh handle.
	 * This method is explicitly called after caching data in the GPU memory prior to rendering.
	 */
	virtual void clear() override final
	{
		vertexes.clear();
		indexes.clear();
	}

	std::vector<float> vertexes;
	std::vector<int> indexes;
};

/**
 * DMKRenderAttachment
 * - DMKMeshHandle (n)
 * - DMKRenderSpecification
 * - DMKModelController
 */
