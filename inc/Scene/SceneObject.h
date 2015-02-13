#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include "BoundingBox.h" // is a bounding box
#include "VertexFormat.h"
#include "GeometryBuffer.h"

namespace Candy
{
	class RenderDevice;
	namespace Scene
	{
		struct ScenePath
		{
			std::vector<glm::vec3> positions;
			std::vector<glm::vec3> directions;
			std::vector<float> timestamps;
		};
		class SceneObject : public BoundingBox
		{
		public:

			//Default Constructor initializes matrix to 1
			SceneObject();
			SceneObject(const glm::mat4& m4WorldMatrix);
			~SceneObject();

			virtual void update(double dTime);

			const glm::mat4& getWorldMatrix() { return m4WorldMatrix; };
			void setWorldMatrix(const glm::mat4& m4NewWorldMatrix) { m4WorldMatrix = m4NewWorldMatrix; };

			const glm::vec3 getPosition();

			void cubic_hermite(float p0, float p1, float m0, float m1, float t, float& value, float& tangent);
			glm::mat4 hermiteSpline(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& directions, float t);
      glm::mat4 catmulRom(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& directions, float t);
      glm::vec3 catmulRomPositionOnly(const std::vector<glm::vec3>& points, float t);
			glm::vec3 hermiteSplinePosition(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& directions, float t);

			void addNodeToPath(const glm::vec3& position, const glm::vec3& direction);
      void addLerpToPath(int p);
      glm::vec3 getLastPathPosition();
      void clearPath();

			bool hasPath(){ return !scene_path.positions.empty(); };

			void genTrail();

			void drawTrail(Candy::RenderDevice* pRD);

			

		protected:
			glm::mat4 m4WorldMatrix;
			Candy::Scene::ScenePath scene_path;

		private:
			Candy::VertexFormat* simpleVFormat;
			Candy::GeometryBuffer* m_trail;
		};

	}
}

