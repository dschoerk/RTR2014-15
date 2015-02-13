#pragma once
#include <GL/glew.h>

namespace Candy
{
  enum PrimitiveMode
  {
    TRIANGLES = GL_TRIANGLES,
    LINES = GL_LINES,
    LINES_STRIP = GL_LINE_STRIP,
    POINTS = GL_POINTS,
    PATCHES = GL_PATCHES
  };
}