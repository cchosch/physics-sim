#include <GL/glew.h>
#include "VertexBuffer.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size) {
    glSafeCall(glGenBuffers(1, &m_Renderer_ID)); // generate buffer and then get id for buffer
    this->Bind();
    glSafeCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW)); // fill bound buffer with data 
}

VertexBuffer::~VertexBuffer() {
    glSafeCall(glDeleteBuffers(1, &m_Renderer_ID));
}
void VertexBuffer::Bind() {
    glSafeCall(glBindBuffer(GL_ARRAY_BUFFER, m_Renderer_ID));
}
void VertexBuffer::Unbind(){
    glSafeCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
