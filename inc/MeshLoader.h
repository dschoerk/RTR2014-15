#pragma once

/**
 * H file for the mesh loader
 * @author David Pfahler
 */

// Library Includes
#include <string>
#include <glm/glm.hpp>
#include <assimp/scene.h>

// project includes
#include "Scene/SceneShape.h"


namespace Candy
{

	class MeshLoader
	{
	public:

		// Type Definitions
		struct Vertex
    {
      glm::vec3 v3Position;
      glm::vec3 v3Normal;
      glm::vec2 v2Texcoord;
      glm::ivec4 v4BoneIds;
      glm::vec4 v4BoneWeights;
    };

    struct SimpleVertex
    {
      glm::vec3 v3Position;
      glm::vec3 v3Normal;
      glm::vec2 v2Texcoord;
    };

		//Constructor
		MeshLoader();

		//Destructor
		virtual ~MeshLoader();

		/**
		 * Loads a Mesh and all its Textures with Assimp
		 * @param szFilename path to the model
		 * @return a SceneObject that contains the mesh and the assets
		 */
		Scene::SceneShape* LoadMesh(const std::string& szFilename);

	private:
		Material* ItlLoadMaterial(const aiScene* pScene, const unsigned int nMaterialIndex, const std::string& szFilename);
	};

}

