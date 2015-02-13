#pragma once

/**
* H file for a scene shape scene object
* @author David Pfahler
*/

#include "SceneObject.h" //is a SceneObject

// library includes
#include <vector>

// project includes
#include "GLTexture.h"
#include "VertexFormat.h"
#include "Geometry.h"
#include "RenderDevice.h"
#include "GLProgram.h"
#include "Material.h"

namespace Candy
{

	namespace Scene
	{

		class SceneShape : public SceneObject
		{
		public:

			SceneShape(const glm::mat4& m4WorldMatrix);
			virtual ~SceneShape();

			void draw(RenderDevice* pRD);
			void AddMeshPart(Geometry* pGeometry, Material* pMaterial);
			void SetVertexFormatOfGeometry(VertexFormat* pVertexFormat);
			void SetRenderingTechnique(GL::GLProgram* val);
			void UploadGeometrie();
			void SetGeometryBuffer(Candy::GeometryBuffer* val) { pGeometryBuffer = val; }

      void SetCulling(bool bSetCulling){ m_bCulling = bSetCulling; };

		//private:
			std::vector<Geometry*> m_vpMeshParts;
			std::vector<Material*> m_vpMaterials;
			Candy::GeometryBuffer* pGeometryBuffer;
			VertexFormat* m_pVertexFormat;
			GL::GLProgram* m_pRenderingTechnique;

			unsigned int nNumberMeshes;
      bool m_bCulling;
		};
	}
}

