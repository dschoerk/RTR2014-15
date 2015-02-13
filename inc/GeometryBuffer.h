#pragma once
#include <GLBuffer.h>

namespace Candy
{
  class GeometryBuffer
  {
    friend class RenderDevice;

  public:
    GeometryBuffer():vbuffer(nullptr), ibuffer(nullptr){};
    ~GeometryBuffer();
    
    template <typename T, typename I>
    void upload(std::vector<T> vertices, std::vector<I> indices);

    template <typename T>
    void upload(std::vector<T> vertices);

  private:
    GL::GLVertexBuffer* vbuffer;
    GL::GLIndexBuffer* ibuffer;
  };

  template <typename T, typename I>
  void GeometryBuffer::upload(std::vector<T> vertices, std::vector<I> indices)
  {
    vbuffer = new GL::GLVertexBuffer;
    vbuffer->upload(vertices);
    ibuffer = new GL::GLIndexBuffer;
    ibuffer->upload(indices);
  }

  template <typename T>
  void GeometryBuffer::upload(std::vector<T> vertices)
  {
    vbuffer = new GL::GLVertexBuffer;
    vbuffer->upload(vertices);
  }
}