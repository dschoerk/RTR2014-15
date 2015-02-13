#pragma once

#include <GL/glew.h>
#include <Datatypes.h>
#include <GL/GLBuffer.h>

namespace Candy
{

  class VertexFormat
  {
  public:
    explicit VertexFormat();
    void addVertexElement(int attributeIndex, FieldType fieldType, int byteOffset, AttributeUsage usage);
    void bind();
    void unbind();
    void bindVertexBuffer(GL::GLVertexBuffer* vbuffer);
    unsigned getFormatSize();

  private:
    GLuint handle;
    GLuint formatSize;
  };
}