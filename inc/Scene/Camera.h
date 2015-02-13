#pragma once

#include "SceneObject.h" //is a SceneObject

namespace Candy
{
	namespace Scene
	{
   

		class Camera : public SceneObject
		{
		public:
			Camera(GLFWwindow* pWindow, glm::vec3 v3Position, int width, int height);
			~Camera();

			virtual void updateKeyboard(double dDeltaTime);
			virtual void updatePath(double dDeltaTime);

			bool inFrustum(Scene::BoundingBox const &boundingBox);

			bool ItlInFrustum(glm::vec3 const &oCenter, const double dRadius);

			glm::mat4 getViewMatrix() const { return m4ViewMatrix; };
			glm::mat4 getProjectionMatrix() const { return m4ProjectionMatrix; };
			glm::mat4 getWVPMatrix() const { return m4WVP; };
			unsigned int getNumberOfShownObjects() const { return nNumShownObjects; };

			void setPosition(glm::vec3 _v3Position);
      const glm::vec3 getDirection();

		private:

			// A plane in 3D (defined by 4 floats)
			typedef struct {
				float A, B, C, D;
			} Plane;

			// View Frustum (defined by 6 planes)
			typedef struct {
				Plane planes[6];
			} Frustum;

			void ItlMove(glm::vec3 const &direction, const double dDeltaTime);

			/**
			 * calculates the new direction of the camera by getting the input of the window
			 * @return the new normalized direction of the camera
			 */
			glm::vec3 ItlCalculateDirectionByMouse();

			void ItlExtractPlane(Plane &plane, int iRow);

			// the window to get the inputs from
			GLFWwindow* pWindow;

			//camera attributes
			float		fFieldOfView, 
						fSpeed, 
						fMouseSpeed,
						fHorizontalAngle, 
						fVerticalAngle;
			int			width,
						height;
			glm::vec3	v3Direction,
						v3Right,
						v3Up,
						v3Position;

			glm::mat4	m4ProjectionMatrix,
						m4ViewMatrix,
						m4WVP;

			Frustum frustum;

			unsigned int nNumShownObjects;
		};

	}
}

