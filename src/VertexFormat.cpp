#include <VertexFormat.h>

Candy::VertexFormat::VertexFormat()
{
  formatSize = 0;
  glGenVertexArrays(1, &handle);
}

void Candy::VertexFormat::addVertexElement(int attributeIndex, FieldType fieldType, int byteOffset, AttributeUsage usage)
{
  bind();
  glEnableVertexAttribArray(attributeIndex);

  if(isInt(fieldType))
    glVertexAttribIFormat(attributeIndex, components(fieldType), basetype(fieldType), byteOffset);
  else
    glVertexAttribFormat(attributeIndex, components(fieldType), basetype(fieldType), GL_FALSE, byteOffset);


  glVertexAttribBinding(attributeIndex, 0);
  unbind();

  formatSize += byteSize(fieldType);
}

void Candy::VertexFormat::bind()
{
  glBindVertexArray(handle);
}

void Candy::VertexFormat::unbind()
{
  glBindVertexArray(0);
}

void Candy::VertexFormat::bindVertexBuffer(GL::GLVertexBuffer* vbuffer)
{
  glBindVertexBuffer(0, vbuffer->getHandle(), 0, formatSize);
}

unsigned Candy::VertexFormat::getFormatSize()
{
  return formatSize;
}