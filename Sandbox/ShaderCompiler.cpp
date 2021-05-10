#include <string>
#include <vector>

enum class ShaderLocation : unsigned char {
	VERTEX, TESSELLATION, GEOMETRY, FRAGMENT, COMPUTE
};

enum class DataType : unsigned char {
	UNDEFINED,

	FLOAT,
	INT,
	SINT,
	UINT,

	VECTOR_2_FLOAT,
	VECTOR_2_INT,
	VECTOR_2_SINT,
	VECTOR_2_UINT,

	VECTOR_3_FLOAT,
	VECTOR_3_INT,
	VECTOR_3_SINT,
	VECTOR_3_UINT,

	VECTOR_4_FLOAT,
	VECTOR_4_INT,
	VECTOR_4_SINT,
	VECTOR_4_UINT,
};

enum class ShaderVersion : unsigned char {
	OLD, LATEST
};

class ShaderCode {
	struct Attribute {
		Attribute() = default;
		Attribute(DataType type, const char* pName, size_t layers)
			: mDataType(type), pName(pName), mLayers(layers) {}

		const char* pName = nullptr;
		size_t mLayers = 1;
		DataType mDataType = DataType::UNDEFINED;
	};

public:
	ShaderCode() = default;
	ShaderCode(ShaderLocation location, ShaderVersion version = ShaderVersion::LATEST)
		: mLocation(location), mVersion(version) {}
	~ShaderCode() {}

	void AddInputAttribute(DataType type, const char* pName, size_t arrayCount = 1)
	{
		mInputAttributes.insert(mInputAttributes.end(), Attribute(type, pName, arrayCount));
	}

	void AddOutputAttribute(DataType type, const char* pName, size_t arrayCount = 1)
	{
		mOutputAttributes.insert(mOutputAttributes.end(), Attribute(type, pName, arrayCount));
	}

	std::string GetAsGLSL() { return std::string(); }
	std::string GetAsHLSL() { return std::string(); }
	std::string GetAsSPIRV() { return std::string(); }

	std::vector<Attribute> mInputAttributes;
	std::vector<Attribute> mOutputAttributes;

	ShaderLocation mLocation = ShaderLocation::VERTEX;
	ShaderVersion mVersion = ShaderVersion::LATEST;
};