#include <GL/GLCubeMap.h>

#include <FreeImage.h>
#include <iostream>
#include <memory>

Candy::GL::GLCubeMap::GLCubeMap()
{
	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_CUBE_MAP, handle);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	initVao();
}

Candy::GL::GLCubeMap::~GLCubeMap()
{
	glDeleteTextures(1, &handle);
	glDeleteVertexArrays(1, &vao);
}

void Candy::GL::GLCubeMap::initVao(){
	float points[] = {
		-10.0f, 10.0f, -10.0f,
		-10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,
		10.0f, 10.0f, -10.0f,
		-10.0f, 10.0f, -10.0f,

		-10.0f, -10.0f, 10.0f,
		-10.0f, -10.0f, -10.0f,
		-10.0f, 10.0f, -10.0f,
		-10.0f, 10.0f, -10.0f,
		-10.0f, 10.0f, 10.0f,
		-10.0f, -10.0f, 10.0f,

		10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, 10.0f,
		10.0f, 10.0f, 10.0f,
		10.0f, 10.0f, 10.0f,
		10.0f, 10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,

		-10.0f, -10.0f, 10.0f,
		-10.0f, 10.0f, 10.0f,
		10.0f, 10.0f, 10.0f,
		10.0f, 10.0f, 10.0f,
		10.0f, -10.0f, 10.0f,
		-10.0f, -10.0f, 10.0f,

		-10.0f, 10.0f, -10.0f,
		10.0f, 10.0f, -10.0f,
		10.0f, 10.0f, 10.0f,
		10.0f, 10.0f, 10.0f,
		-10.0f, 10.0f, 10.0f,
		-10.0f, 10.0f, -10.0f,

		-10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f, 10.0f,
		10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f, 10.0f,
		10.0f, -10.0f, 10.0f
	};
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof(float), &points, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}

void Candy::GL::GLCubeMap::bind(int unit) const
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, handle);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

bool Candy::GL::GLCubeMap::loadFromFile(char const* szPath, bool bSwapRB = false)
{
	const std::string szFacename[6] = {
		"posx.jpg", //RIGHT 
		"negx.jpg", //LEFT

		"posy.jpg", //TOP
		"negy.jpg", //BOTTOM

		"posz.jpg", //BACK
		"negz.jpg", //FRONT
	};

	const GLenum eTypes[6] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	for (int iFaceID = 0; iFaceID < 6; ++iFaceID){
		std::string sImagePath = szPath + szFacename[iFaceID];
		const char* szPath = sImagePath.c_str();
		FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
		FIBITMAP* dib(nullptr);

		fif = FreeImage_GetFileType(szPath, 0); // Check the file signature and deduce its format

		if (fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
			fif = FreeImage_GetFIFFromFilename(szPath);

		if (fif == FIF_UNKNOWN) // If still unkown, return failure
			return false;

		if (FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
			dib = FreeImage_Load(fif, szPath);
		if (!dib)
			return false;

		BYTE* bDataPointer = FreeImage_GetBits(dib); // Retrieve the image data

		unsigned iWidth = FreeImage_GetWidth(dib); // Get the image width and height
		unsigned iHeight = FreeImage_GetHeight(dib);
		unsigned iBPP = FreeImage_GetBPP(dib);

		// If somehow one of these failed (they shouldn't), return failure
		if (bDataPointer == NULL || iWidth == 0 || iHeight == 0)
			return false;

		glBindTexture(GL_TEXTURE_2D, handle);
		GLenum _24bitFmt = bSwapRB ? GL_BGR : GL_RGB;
		int iFormat = iBPP == 32 ? GL_RGBA : iBPP == 24 ? _24bitFmt : iBPP == 8 ? GL_LUMINANCE : 0;
		int iInternalFormat = iBPP == 32 ? GL_RGBA : iBPP == 24 ? GL_RGB : GL_DEPTH_COMPONENT;
		glTexImage2D(eTypes[iFaceID], 0, iInternalFormat, iWidth, iHeight, 0, iFormat, GL_UNSIGNED_BYTE, bDataPointer);
		glBindTexture(GL_TEXTURE_2D, 0);

		FreeImage_Unload(dib);
	}
	
	return true;
}