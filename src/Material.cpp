#include "Material.h"
#include "GLTexture.h"
#include <iostream>
#include <assert.h>

Candy::Material::Material()
{
	for (int i = 0; i < CANDY_TEXTURE_NUM; i++)
	{
		m_pTexture[i] = nullptr;
	}
}

Candy::Material::~Material()
{
	for (int i = 0; i < CANDY_TEXTURE_NUM; i++)
	{
		if (m_pTexture[i] != nullptr)
		{
			delete m_pTexture[i];
			m_pTexture[i] = nullptr;
		}
	}
}

bool Candy::Material::LoadTexture(std::string& szFilename, TextureType eTextureType)
{
	if (eTextureType == CANDY_TEXTURE_NUM)
	{
		std::cout << "Invalid texture type" << std::endl;
		return false;
	}
	Candy::GL::GLTexture* pTexture = new Candy::GL::GLTexture();
	if (!pTexture->loadFromFile(szFilename.c_str(), true))
	{
		std::cout
			<< "Error loading texture: "
			<< szFilename.c_str()
			<< std::endl;
	}
	if (m_pTexture[eTextureType] == nullptr)
		m_pTexture[eTextureType] = pTexture;
	else{
		std::cout << "This texture type was already assigned" << std::endl;
		return false;
	}
	return true;
}

void Candy::Material::SetUniforms(GL::GLProgram* pRenderingTechnique)
{

	char * szUniforms[] = { "diffuse", "normal", "specular", "alpha" };
	char * szBoolUniforms[] = {"", "normalMappingOn", "specularMappingOn", "alphaMappingOn"};
	for (int eTextureType = 0; eTextureType < CANDY_TEXTURE_NUM; eTextureType++)
	{
		if (m_pTexture[eTextureType] != nullptr)
		{
			if (eTextureType != CANDY_TEXTURE_DIFFUSE)
				pRenderingTechnique->setUniform(szBoolUniforms[eTextureType], 1);
			pRenderingTechnique->setUniform(szUniforms[eTextureType], m_pTexture[eTextureType], eTextureType);
		}
		else if (eTextureType != CANDY_TEXTURE_DIFFUSE)
		{
			pRenderingTechnique->setUniform(szBoolUniforms[eTextureType], 0);
		} 
	}
}

