#pragma once
#include <string>
#include "GLTexture.h"
#include "GLProgram.h"

namespace Candy
{
	class Material
	{
	public:
		// Type Definitions
		enum TextureType
		{
			CANDY_TEXTURE_DIFFUSE,
			CANDY_TEXTURE_NORMAL,
			CANDY_TEXTURE_SPECULAR,
			CANDY_TEXTURE_ALPHA,
			CANDY_TEXTURE_NUM
		};

		Material();
		~Material();
		bool LoadTexture(std::string& szFilename, TextureType eTextureType);
		void SetUniforms(GL::GLProgram* pRenderingTechnique);

	private:
		GL::GLTexture* m_pTexture[CANDY_TEXTURE_NUM];
	};
}