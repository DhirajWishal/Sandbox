#pragma once

#include "String.h"

struct ShaderModule {};
ShaderModule LoadFromFile(const String& asset) { return ShaderModule(); }

struct VertexDescriptor {};
struct MaterialDescriptor {};

struct FunctionDescription {
	String functionName = "";
};

enum class ShaderLocation {
	SL_VERTEX,
	SL_TESSELLATION,
	SL_GEOMETRY,
	SL_FRAGMENT,
};

class SmartShaderCompiler {
public:
	SmartShaderCompiler() {}
	~SmartShaderCompiler() {}

	void AddDictionary(const ShaderModule& shaderModule) {}
	ShaderModule CreateShader(VertexDescriptor* pVertexDescriptor, MaterialDescriptor* pMaterialDescriptor, ShaderLocation location) { return ShaderModule(); }
};
