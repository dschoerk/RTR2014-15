#include <GL/GLTexture.h>
#include <string>

Candy::GL::GLTexture::GLTexture()
{
  glGenTextures(1, &handle);
  glBindTexture(GL_TEXTURE_2D, handle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
}

Candy::GL::GLTexture::~GLTexture()
{
  glDeleteTextures(1, &handle);
}

void Candy::GL::GLTexture::bind(int texture_slot) const
{
	glActiveTexture(GL_TEXTURE0 + texture_slot);
	glBindTexture(GL_TEXTURE_2D, handle);

}

bool Candy::GL::GLTexture::loadFromFile(char const* path, bool swapRB=false)
{
  std::string _p = std::string(path);

  FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
  FIBITMAP* dib(nullptr);

  fif = FreeImage_GetFileType(path, 0); // Check the file signature and deduce its format

  if (fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
    fif = FreeImage_GetFIFFromFilename(path);

  if (fif == FIF_UNKNOWN) // If still unkown, return failure
    return false;

  if (FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
    dib = FreeImage_Load(fif, path);
  if (!dib)
    return false;

  BYTE* bDataPointer = FreeImage_GetBits(dib); // Retrieve the image data

  unsigned iWidth = FreeImage_GetWidth(dib); // Get the image width and height
  unsigned iHeight = FreeImage_GetHeight(dib);
  unsigned iBPP = FreeImage_GetBPP(dib);

  // If somehow one of these failed (they shouldn't), return failure
  if (bDataPointer == NULL || iWidth == 0 || iHeight == 0)
    return false;

  // Generate an OpenGL texture ID for this texture
  glBindTexture(GL_TEXTURE_2D, handle);
  GLenum _24bitFmt = swapRB ? GL_BGR : GL_RGB;
  int iFormat = iBPP == 32 ? GL_RGBA : iBPP == 24 ? _24bitFmt : iBPP == 8 ? GL_LUMINANCE : 0;
  int iInternalFormat = iBPP == 32 ? GL_RGBA : iBPP == 24 ? GL_RGB : GL_DEPTH_COMPONENT;
  glTexImage2D(GL_TEXTURE_2D, 0, iInternalFormat, iWidth, iHeight, 0, iFormat, GL_UNSIGNED_BYTE, bDataPointer);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  FreeImage_Unload(dib);
  return true;
}