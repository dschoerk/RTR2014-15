#pragma once

#include <GL/glew.h>
#include <vector>

namespace Candy
{
  namespace GL
  {
    template<GLuint BUFFER_TYPE>
    class GLBuffer
    {
    public:
      GLBuffer();
      ~GLBuffer();
      void bind();
      void unbind();
      void upload(void const* data, std::size_t size);
      template<typename T>
      void upload(const std::vector<T>& data);
      GLuint getHandle();
      size_t getNumberOfElements();

    private:
      GLuint handle;
      size_t numElements;
    };

    template <unsigned BUFFER_TYPE>
    template <typename T>
    void GLBuffer<BUFFER_TYPE>::upload(std::vector<T> const& data)
    {
      upload(&data[0], sizeof(T)* data.size());
    }

    template <unsigned BUFFER_TYPE>
    GLBuffer<BUFFER_TYPE>::GLBuffer()
    {
      glGenBuffers(1, &handle);
    }

    template <unsigned BUFFER_TYPE>
    GLBuffer<BUFFER_TYPE>::~GLBuffer()
    {
      glDeleteBuffers(1, &handle);
    }

    template <unsigned BUFFER_TYPE>
    void GLBuffer<BUFFER_TYPE>::bind()
    {
      glBindBuffer(BUFFER_TYPE, handle);
    }

    template <unsigned BUFFER_TYPE>
    void GLBuffer<BUFFER_TYPE>::unbind()
    {
      glBindBuffer(BUFFER_TYPE, 0);
    }

    template <unsigned BUFFER_TYPE>
    void GLBuffer<BUFFER_TYPE>::upload(void const* data, std::size_t size)
    {
      bind();
      glBufferData(BUFFER_TYPE, size, data, GL_STATIC_DRAW);
      numElements = size;
      unbind();
    }

    template <unsigned BUFFER_TYPE>
    GLuint GLBuffer<BUFFER_TYPE>::getHandle()
    {
      return handle;
    }

    template <unsigned BUFFER_TYPE>
    size_t GLBuffer<BUFFER_TYPE>::getNumberOfElements()
    {
      return numElements;
    }

    typedef class GLBuffer<GL_ARRAY_BUFFER> GLVertexBuffer;
    typedef class GLBuffer<GL_ELEMENT_ARRAY_BUFFER> GLIndexBuffer;
  }
}