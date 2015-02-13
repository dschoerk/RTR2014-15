#include <GeometryBuffer.h>

Candy::GeometryBuffer::~GeometryBuffer()
{
  if (vbuffer != nullptr)
    delete vbuffer;

  if (ibuffer != nullptr)
    delete ibuffer;
}