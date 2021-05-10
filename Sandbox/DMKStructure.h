struct VertexBuffer {};
struct IndexBuffer {};
struct UniformBuffer {};
struct Material {};

struct MeshHandle {
	VertexBuffer vertexBuffer = {};
	IndexBuffer indexBuffer = {};
	Material materials[1] = {};
};

class StaticModel {
public:
	StaticModel() {}
	~StaticModel() {}

	UniformBuffer uniformBuffers[1] = {};
	MeshHandle meshHandles[1] = {};
};

class GameEntity {
public:
	GameEntity() {}
	virtual ~GameEntity() {}


};
