#pragma once
class IndexBuffer {
private:
	unsigned int m_Renderer_ID;
	unsigned int m_Count;
public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer(); // destructor

	void Bind();
	void Unbind();
	inline unsigned int getCount() const { return m_Count; };
};