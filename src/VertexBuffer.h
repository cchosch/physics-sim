#pragma once


class VertexBuffer {
private:
	unsigned int m_Renderer_ID;
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer(); // destructor

	void Bind();
	void Unbind();
};


