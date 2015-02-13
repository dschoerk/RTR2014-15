#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "SceneObject.h" //is a SceneObject

namespace Candy
{
	namespace Scene
	{
		enum PointlightDirections
		{
			POSX = 0,
			NEGX, POSY, NEGY, POSZ, NEGZ
		};

		class Pointlight : public SceneObject
		{
		public:
			Pointlight(){}
			void initialize(unsigned size = 256);
			const glm::mat4 getMVP(PointlightDirections dir);
			const glm::mat4 getMV(PointlightDirections dir);

			void update(PointlightDirections dir);
			void updatePath(double dTime);

			glm::vec3 getColor();

			bool inFrustum(Scene::BoundingBox const &boundingBox, unsigned dir);

			//private:
			// A plane in 3D (defined by 4 floats)
			typedef struct {
				float A, B, C, D;
			} Plane;

			// View Frustum (defined by 6 planes)
			typedef struct {
				Plane planes[6];
			} Frustum;

			void ItlExtractPlane(Plane &plane, int iRow);

			GLuint m_fbo[2];
			GLuint m_shadowMap;
			GLuint m_shadowMapBlurred;
			GLuint m_depth;
			GLuint m_tmpTex;
			unsigned fboSize;

			glm::vec3 position;
			glm::vec3 color = glm::vec3(1, 1, 1);
			std::vector<glm::mat4> mvp; // interleaved, [tangent binormal normal], [...], ...
			glm::mat4 m4projection;
			glm::mat4 m4WVP;

			Frustum frustum;
		};
	}
}