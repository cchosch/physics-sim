#include <GL/glew.h>
#include <iostream>
#include "IndexBuffer.h"
#include "Renderer.h"
IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count) : m_Count(count) {

    glSafeCall(glGenBuffers(1, &m_Renderer_ID)); // generate buffer and then get id for buffer
    this->Bind();
    glSafeCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*sizeof(unsigned int), data, GL_STATIC_DRAW)); // fill bound buffer with data 
}

IndexBuffer::~IndexBuffer() {
    glSafeCall(glDeleteBuffers(1, &m_Renderer_ID));
}
void IndexBuffer::Bind() {
    std::cout << m_Renderer_ID << std::endl;
    glSafeCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_Renderer_ID));
}
void IndexBuffer::Unbind(){
    glSafeCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
