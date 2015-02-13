#include "MeshLoader.h"

// Library Includes
#include <iostream>
#include <vector>

// Assimp Includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Project Includes
#include "Scene/SceneShape.h"
#include "Mesh.h"
#include "GeometryBuffer.h"
#include "Geometry.h"
#include "GL/GLTexture.h"
#include "VertexFormat.h"

Candy::MeshLoader::MeshLoader()
{
}


Candy::MeshLoader::~MeshLoader()
{
}

bool has_suffix(const std::string &str, const std::string &suffix)
{
  return str.size() >= suffix.size() &&
    str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

Candy::Scene::SceneShape* Candy::MeshLoader::LoadMesh(const std::string& szFilename)
{
	IndexedMesh<MeshLoader::Vertex, unsigned int> mesh;
	std::vector<Geometry> vMeshParts;
	Scene::SceneShape* pSceneShape = new Scene::SceneShape(glm::mat4(1));
	Assimp::Importer importer;

	// load the file into the scene
  const aiScene* pLoadedScene = importer.ReadFile(szFilename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_GenSmoothNormals);

	if (!pLoadedScene)
	{
		std::cout << "model loading failed" << std::endl;
		return pSceneShape;
	}

  // load first animation if available
  bool bAnimated = false;
  const int numAnim = pLoadedScene->mNumAnimations;
  if (numAnim > 0)
  {
    std::cout << "has animations!" << std::endl;
    aiAnimation* anim = pLoadedScene->mAnimations[0];
    bAnimated = true;
  }

	unsigned numVertices = 0;
	unsigned numIndices = 0;

	Candy::GeometryBuffer* geometry = new Candy::GeometryBuffer;
	pSceneShape->SetGeometryBuffer(geometry);

	// init bounding box
	Scene::SceneObject::BoundingBox modelBoundingBox = Scene::SceneObject::BoundingBox();
	glm::vec3 v3ModelMin = glm::vec3(FLT_MAX);
	glm::vec3 v3ModelMax = glm::vec3(-FLT_MAX);

	//first load all materials
	std::vector<Candy::Material*> materials;
 	materials.reserve(pLoadedScene->mNumMaterials);

	for (unsigned int i = 0; i < pLoadedScene->mNumMaterials; i++)
	{
		materials.push_back(ItlLoadMaterial(pLoadedScene, i, szFilename));
	}
		

	//Set a Vertex Format for the Geometry
  Candy::VertexFormat* pVertexFormat = new Candy::VertexFormat();
  if (!bAnimated)
  {
    pVertexFormat->addVertexElement(0, Candy::FLOAT_VEC3, 0, Candy::POSITION);
    pVertexFormat->addVertexElement(1, Candy::FLOAT_VEC3, 12, Candy::NORMAL);
    pVertexFormat->addVertexElement(3, Candy::FLOAT_VEC2, 24, Candy::TEXCOORD2D);
  }
  else
  {
    pVertexFormat->addVertexElement(0, Candy::FLOAT_VEC3, 0, Candy::POSITION);
    pVertexFormat->addVertexElement(1, Candy::FLOAT_VEC3, 12, Candy::NORMAL);
    pVertexFormat->addVertexElement(3, Candy::FLOAT_VEC2, 24, Candy::TEXCOORD2D);
    pVertexFormat->addVertexElement(4, Candy::INT_VEC4, 32, Candy::TEXCOORD2D);
    pVertexFormat->addVertexElement(5, Candy::FLOAT_VEC4, 48, Candy::TEXCOORD2D);
  }
	pSceneShape->SetVertexFormatOfGeometry(pVertexFormat);

	// check for the number of vertices and indices
	for (unsigned m = 0; m < pLoadedScene->mNumMeshes; m++)
	{
		aiMesh* pLoadedMesh = pLoadedScene->mMeshes[m];
		numVertices += pLoadedMesh->mNumVertices;
		numIndices += pLoadedMesh->mNumFaces * 3;
	}
	mesh.vertices.resize(numVertices);
	mesh.indices.resize(numIndices);


	int vidx_off = 0;
	int iidx_off = 0;
	for (unsigned m = 0; m < pLoadedScene->mNumMeshes; m++)
	{
		aiMesh* pLoadedMesh = pLoadedScene->mMeshes[m];

		//values initialization for bounding box
		glm::vec3 v3Min = glm::vec3(FLT_MAX);
		glm::vec3 v3Max = glm::vec3(-FLT_MAX);

		for (unsigned i = 0; i < pLoadedMesh->mNumVertices; i++)
		{
			const aiVector3D& v = pLoadedMesh->mVertices[i];
			//std::cout << v.x << " " << v.y << " " << v.z << std::endl;
			const aiVector3D& n = pLoadedMesh->mNormals[i];
			const aiVector3D& t = pLoadedMesh->mTextureCoords[0][i];

			Candy::MeshLoader::Vertex vertex;
			vertex.v3Position = { v.x, v.y, v.z };

			//update AABB values
			v3Min = glm::min(v3Min, vertex.v3Position);
			v3Max = glm::max(v3Max, vertex.v3Position);

			if (pLoadedMesh->mNormals)
				vertex.v3Normal = { n.x, n.y, n.z };

			if (pLoadedMesh->mTextureCoords[0])
				vertex.v2Texcoord = { t.x, t.y };

			mesh.vertices[i + vidx_off] = vertex;
		}

		for (unsigned i = 0; i < pLoadedMesh->mNumFaces; i++)
		{
			const aiFace& v = pLoadedMesh->mFaces[i];
			for (int f = 0; f < 3; f++)
			{
				mesh.indices[iidx_off + i * 3 + f] = vidx_off + v.mIndices[f];
			}
		}

		//update AABB values from model
		v3ModelMin = glm::min(v3Min, v3ModelMin);
		v3ModelMax = glm::max(v3Max, v3ModelMax);
		
		Candy::Geometry* pGeometry = new Candy::Geometry(geometry, iidx_off, pLoadedMesh->mNumFaces * 3, v3Min, v3Max);
		pSceneShape->AddMeshPart(pGeometry, materials[pLoadedMesh->mMaterialIndex]);

		vidx_off += pLoadedMesh->mNumVertices;
		iidx_off += pLoadedMesh->mNumFaces * 3;

    // load skeleton if available
    const auto* _mesh = pLoadedScene->mMeshes[m];
    if (_mesh->HasBones())
    {
      for (auto& v : mesh.vertices)
      {
        for (int i = 0; i < 4; i++)
        {
          v.v4BoneWeights[i] = 0;
          v.v4BoneIds[i] = -1;
        }
      }

      for (unsigned b = 0; b < _mesh->mNumBones; b++)
      {
        aiBone* bone = _mesh->mBones[b];
        std::cout << bone->mName.C_Str() << std::endl;
        /*const aiMatrix4x4 aiInvBindPose = bone->mOffsetMatrix;
        glm::mat4 invBindPose;
        copy_mat(&aiInvBindPose, invBindPose);
        bones.push_back(invBindPose);*/
        for (int w = 0; w < bone->mNumWeights; w++)
        {
          const aiVertexWeight weight = bone->mWeights[w];
          Vertex& v = mesh.vertices[weight.mVertexId];

          for (int i = 0; i < 4; i++)
          {
            if (v.v4BoneIds[i] == -1)
            {
              v.v4BoneIds[i] = b;
              v.v4BoneWeights[i] = weight.mWeight;
              break;
            }
          }
        }
      }

/*      for (auto& v : mesh.vertices)
      {
        for (int i = 0; i < 4; i++)
        {
          std::cout << v.v4BoneWeights[i] << "\t" << v.v4BoneIds[i] << "\t";
        }
        std::cout << std::endl;
      }*/
    }
 	}

  if (!bAnimated)
  {
    std::vector<SimpleVertex> verts;
    verts.reserve((mesh.vertices.size()));
    for (const auto& v : mesh.vertices)
    {
      SimpleVertex sv;
      sv.v2Texcoord = v.v2Texcoord;
      sv.v3Normal = v.v3Normal;
      sv.v3Position = v.v3Position;
      verts.push_back(sv);
    }

    geometry->upload(verts, mesh.indices);
  }
  else
  {
    geometry->upload(mesh.vertices, mesh.indices);

    /*for (const auto& v : mesh.vertices)
    {
      std::cout << v.v4BoneIds[0] << " " << v.v4BoneIds[1] << " " << v.v4BoneIds[2] << " " << v.v4BoneIds[3] << std::endl;
    }*/
  }
    
	pSceneShape->SetBoundingBox(v3ModelMin, v3ModelMax);
	// upload the vertices and the indices to the Geometry Buffer
	

	return pSceneShape;
}

Candy::Material* Candy::MeshLoader::ItlLoadMaterial(const aiScene* pScene, const unsigned int nMaterialIndex, const std::string& szFilename)
{
	bool bOk = true;
	Material* pMaterial = new Candy::Material();

	std::string::size_type SlashIndex = szFilename.find_last_of("/");
	std::string Dir;

	if (SlashIndex == std::string::npos) {
		Dir = ".";
	}
	else if (SlashIndex == 0) {
		Dir = "/";
	}
	else {
		Dir = szFilename.substr(0, SlashIndex);
	}

	const aiMaterial* pLoadedMaterial = pScene->mMaterials[nMaterialIndex];
	
	/*
	With this code you can get different material attributes:

	aiColor3D ambient(0.f, 0.f, 0.f);
	aiColor3D diffuse(0.f, 0.f, 0.f);
	aiColor3D emissive(0.f, 0.f, 0.f);
	aiColor3D specular(0.f, 0.f, 0.f);
	float shininess;

	pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
	pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
	pMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
	pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
	pMaterial->Get(AI_MATKEY_SHININESS, shininess);

	glm::vec3(ambient.r, ambient.g, ambient.b),
	glm::vec3(diffuse.r, diffuse.g, diffuse.b),
	glm::vec3(specular.r, specular.g, specular.b),
	glm::vec3(emissive.r, emissive.g, emissive.b),
	*/

	// Textures

	aiString Path;
	bOk = false;
	if (pLoadedMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path) == AI_SUCCESS) {
		pMaterial->LoadTexture(Dir + "/" + Path.data, Candy::Material::CANDY_TEXTURE_DIFFUSE);
		bOk = true;
	}
	if (pLoadedMaterial->GetTexture(aiTextureType_HEIGHT, 0, &Path) == AI_SUCCESS)
	{
		pMaterial->LoadTexture(Dir + "/" + Path.data, Candy::Material::CANDY_TEXTURE_NORMAL);
		bOk = true;
	}
	if (pLoadedMaterial->GetTexture(aiTextureType_SPECULAR, 0, &Path) == AI_SUCCESS)
	{
		pMaterial->LoadTexture(Dir + "/" + Path.data, Candy::Material::CANDY_TEXTURE_SPECULAR);
		bOk = true;
	}
	if (pLoadedMaterial->GetTexture(aiTextureType_OPACITY, 0, &Path) == AI_SUCCESS)
	{
		pMaterial->LoadTexture(Dir + "/" + Path.data, Candy::Material::CANDY_TEXTURE_ALPHA);
		bOk = true;
	}



	if (!bOk)
	{
		//std::cout << "Could not guess the texture type of textureID = " << nMaterialIndex << std::endl;
	}

	return pMaterial;
}