#include "Scene/SceneShape.h"

// Library Includes
#include <iostream>
#include <assert.h>

Candy::Scene::SceneShape::SceneShape(const glm::mat4& m4WorldMatrix) : Candy::Scene::SceneObject(m4WorldMatrix)
{
	nNumberMeshes = 0;
	m_pRenderingTechnique = nullptr;
	pGeometryBuffer = new Candy::GeometryBuffer();
  m_bCulling = true;
}


Candy::Scene::SceneShape::~SceneShape()
{
	for (auto pGeometry : m_vpMeshParts)
		delete pGeometry;
	for (auto pMaterial : m_vpMaterials)
		delete pMaterial;

	delete m_pVertexFormat;
	delete pGeometryBuffer;
}

void Candy::Scene::SceneShape::draw(Candy::RenderDevice* pRD)
{
	assert(m_pRenderingTechnique != nullptr);
	m_pRenderingTechnique->bind();

	for (unsigned int n = 0; n < m_vpMeshParts.size(); n++)
	{

    if (pRD->getCamera()->inFrustum(*m_vpMeshParts[n]) || !m_bCulling)
    {
      m_vpMaterials[n]->SetUniforms(m_pRenderingTechnique);
      pRD->draw(m_vpMeshParts[n], m_pVertexFormat, m_pRenderingTechnique->getPrimitiveMode());
    }
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}


void Candy::Scene::SceneShape::AddMeshPart(Geometry* pGeometry, Material* pMaterial)
{
	nNumberMeshes++;
	m_vpMeshParts.push_back(pGeometry);
	m_vpMaterials.push_back(pMaterial);
}

void Candy::Scene::SceneShape::SetVertexFormatOfGeometry(VertexFormat* pVertexFormat)
{
	m_pVertexFormat = pVertexFormat;
}

void Candy::Scene::SceneShape::SetRenderingTechnique(GL::GLProgram* val)
{
	m_pRenderingTechnique = val;
}

