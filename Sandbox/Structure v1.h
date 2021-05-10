#pragma once

class VertexBuffer {};
struct VertexBufferRef { VertexBuffer* pVertexBuffer = nullptr; };

class IndexBuffer {};
struct IndexBufferRef { IndexBuffer* pIndexBuffer = nullptr; };

VertexBufferRef InitializeVertexBuffer(VertexBuffer* pVertexBuffer) { return VertexBufferRef(); }
IndexBufferRef InitializeIndexBuffer(IndexBuffer* pIndexBuffer) { return IndexBufferRef(); }
