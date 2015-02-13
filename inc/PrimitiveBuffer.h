#pragma once

#include <gl/glew.h>
#include <iostream>
#include <GL/GLBuffer.h>

namespace Candy
{
  enum DrawMode
  {
    Arrays, // no indices, vertices only
    Indexed, // vertices+indices

  };

  class PrimitiveBuffer
  {
  public:
    explicit PrimitiveBuffer();
    ~PrimitiveBuffer();

    void bind();
    void unbind();
    void setNumElements(unsigned int);
    int getNumElements();

    GLuint getIndexType() { return indexType; }
    void setIndexType(GLuint t) { indexType = t; }

    void initVBuffer() { vbuffer = new GL::GLVertexBuffer(); }
    GL::GLVertexBuffer* getVbuffer() { return vbuffer; }

    void initIBuffer() { ibuffer = new GL::GLIndexBuffer(); }
    GL::GLIndexBuffer* getIbuffer() { return ibuffer; }

  private:
    GLuint handle;
    unsigned int numelements;
    GLuint vbo, ibo;
    GL::GLVertexBuffer* vbuffer;
    GL::GLIndexBuffer* ibuffer;
    GLuint indexType;
  };

  inline PrimitiveBuffer::PrimitiveBuffer() : vbo(-1), ibo(-1), vbuffer(nullptr), ibuffer(nullptr)
  {
    glGenVertexArrays(1, &handle);
  }

  inline PrimitiveBuffer::~PrimitiveBuffer()
  {
    std::cout << "dtor" << std::endl;
    
    if (handle > -1)
      glDeleteVertexArrays(1, &handle);
    if (handle > -1)
      glDeleteBuffers(1, &vbo);
    if (handle > -1)
      glDeleteVertexArrays(1, &ibo);
    if (vbuffer)
      delete vbuffer;
    if (ibuffer)
      delete ibuffer;
  }

  inline void PrimitiveBuffer::bind()
  {
    glBindVertexArray(handle);
  }

  inline void PrimitiveBuffer::unbind()
  {
    glBindVertexArray(0);
  }

  inline void PrimitiveBuffer::setNumElements(unsigned numElements)
  {
    PrimitiveBuffer::numelements = numElements;
  }

  inline int PrimitiveBuffer::getNumElements()
  {
    return PrimitiveBuffer::numelements;
  }
}