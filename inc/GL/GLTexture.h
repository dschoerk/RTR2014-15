#pragma once
#include <GL/glew.h>
#include <FreeImage.h>

namespace Candy
{
  namespace GL
  {
    class GLTexture
    {
    public:
      GLTexture();
      ~GLTexture();
      void bind(int texture_slot) const;
      bool loadFromFile(const char* path, bool swapRB);
    private:
      GLuint handle;
    };
  }
}