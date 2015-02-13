
#define GLM_FORCE_RADIANS
//library include
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/spline.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/ext.hpp>

// math include
#define _USE_MATH_DEFINES	
#include <math.h>

#include <iostream>

//project include
#include <Scene/Camera.h>

#define PI					(float(M_PI))
#define CAMERA_SPEED		(300.0f)
#define CAMERA_MOUSE_SPEED	(0.01f)
#define CAMERA_FOV			(PI/180.0f*45.0f)
#define DISPLAY_RANGE_NEAR	(20.f)
#define DISPLAY_RANGE_FAR	(3000.0f)
#define ASPECT_RATIO		(WIDTH/HEIGHT)

Candy::Scene::Camera::Camera(GLFWwindow* _pWindow, glm::vec3 _v3Position, int _width, int _height) : 
	SceneObject(glm::mat4(1.0f)),
	pWindow(_pWindow),
	v3Position(_v3Position),
	width(_width),
	height(_height)
{
	m4ProjectionMatrix = glm::perspective(CAMERA_FOV, float(width)/float(height), DISPLAY_RANGE_NEAR, DISPLAY_RANGE_FAR);
	v3Up = glm::vec3(0, 1, 0);
	fHorizontalAngle = fVerticalAngle = 0.0f;
  updatePath(0);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Candy::Scene::Camera::~Camera()
{

}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/**
* clamp a value to be greater than a and smaller than b
* @param x the value
* @param a the lower bound
* @param b the higher bound
* @return the clamped value
*/
inline float ItlClamp(float x, float a, float b)
{
	if (x < a){
		return a;
	}
	if (x > b){
		return b;
	}
	return x;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/**
 * if the input is greater than pi it reduces it by 2 pi and if it is smaller than pi it increases it with 2 pi
 * @param r the value to get cirled up
 * @return r in range [-pi,pi]
 */
inline float ItlCircleUp(float r)
{
	if (r > PI){
		r -= 2 * PI;
	}
		if (r < -PI){
		r += 2 * PI;
	}
	return r;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Candy::Scene::Camera::updateKeyboard(double dDeltaTime){

	v3Direction = ItlCalculateDirectionByMouse();
	v3Right = glm::normalize(glm::cross(v3Direction, v3Up));

	// ItlMove forward
	if (glfwGetKey(pWindow, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(pWindow, 'W') == GLFW_PRESS){
		ItlMove(v3Direction, (float)dDeltaTime);
	}
	// ItlMove backward
	if (glfwGetKey(pWindow, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(pWindow, 'S') == GLFW_PRESS){
		ItlMove(-v3Direction, (float)dDeltaTime);
	}
	// Strafe right
	if (glfwGetKey(pWindow, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(pWindow, 'D') == GLFW_PRESS){
		ItlMove(v3Right, (float)dDeltaTime);
	}
	// Strafe left
	if (glfwGetKey(pWindow, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(pWindow, 'A') == GLFW_PRESS){
		ItlMove(-v3Right, (float)dDeltaTime);
	}

	// Camera matrix
	m4ViewMatrix = glm::lookAt(
		glm::vec3(0),			// Camera is here
		v3Direction,		// and looks here : at the same position, plus "direction"
		v3Up                // Head is up
		);

	setWorldMatrix(glm::translate(m4ViewMatrix, -v3Position));

	m4WVP = m4ProjectionMatrix * getWorldMatrix();

	ItlExtractPlane(frustum.planes[0], 1);
	ItlExtractPlane(frustum.planes[1], -1);
	ItlExtractPlane(frustum.planes[2], 2);
	ItlExtractPlane(frustum.planes[3], -2);
	ItlExtractPlane(frustum.planes[4], 3);
	ItlExtractPlane(frustum.planes[5], -3);

	nNumShownObjects = 0;

}

void Candy::Scene::Camera::updatePath(double dDeltaTime)
{
  if (hasPath())
  {
    auto worldMat = catmulRom(scene_path.positions, scene_path.directions, dDeltaTime);
    setWorldMatrix(glm::inverse(worldMat));
  }

  m4WVP = m4ProjectionMatrix * getWorldMatrix();

  ItlExtractPlane(frustum.planes[0], 1);
  ItlExtractPlane(frustum.planes[1], -1);
  ItlExtractPlane(frustum.planes[2], 2);
  ItlExtractPlane(frustum.planes[3], -2);
  ItlExtractPlane(frustum.planes[4], 3);
  ItlExtractPlane(frustum.planes[5], -3);

  nNumShownObjects = 0;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool Candy::Scene::Camera::inFrustum(Scene::BoundingBox const &boundingBox)
{
	glm::vec3 oCenter = boundingBox.position;
	double radius = boundingBox.radius;

	return ItlInFrustum(oCenter, radius);

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Candy::Scene::Camera::setPosition(glm::vec3 _v3Position)
{
	v3Position = _v3Position;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void	Candy::Scene::Camera::ItlMove(glm::vec3 const &v3Direction, const double dDeltaTime){
	glm::vec3 change = v3Direction * float(dDeltaTime) * CAMERA_SPEED;
	//const glm::vec3 tmpDir = glm::vec3(1);
	v3Position += change;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
glm::vec3	Candy::Scene::Camera::ItlCalculateDirectionByMouse()
{
	double dXPos, dYPos;

	// Get mouse position
	glfwGetCursorPos(pWindow, &dXPos, &dYPos);

	// Reset mouse position for next frame
	glfwSetCursorPos(pWindow, (int)width / 2, (int)height / 2);

	//calculate change of 
	const float fDelataX = CAMERA_MOUSE_SPEED * (width / 2.0f - float(dXPos));
	const float fDelataY = CAMERA_MOUSE_SPEED * (height / 2.0f - float(dYPos));

	fHorizontalAngle += fDelataX;
	fVerticalAngle += fDelataY;
	fVerticalAngle = ItlClamp(fVerticalAngle, -PI / 2.0f + 0.1f, PI / 2.0f - 0.1f);
	fHorizontalAngle = ItlCircleUp(fHorizontalAngle);

	glm::vec3 v3Dir = glm::vec3(
		cos(fVerticalAngle) * sin(fHorizontalAngle),
		sin(fVerticalAngle),
		cos(fVerticalAngle) * cos(fHorizontalAngle)
		);

	return glm::normalize(v3Dir);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Candy::Scene::Camera::ItlExtractPlane(Scene::Camera::Plane &rPlane, int iRow)
{
	float scale = (iRow < 0) ? -1.0f : 1.0f;
	iRow = abs(iRow) - 1;

	rPlane.A = m4WVP[0][3] + scale * m4WVP[0][iRow];
	rPlane.B = m4WVP[1][3] + scale * m4WVP[1][iRow];
	rPlane.C = m4WVP[2][3] + scale * m4WVP[2][iRow];
	rPlane.D = m4WVP[3][3] + scale * m4WVP[3][iRow];
	// normalize the plane
	float length = sqrtf(rPlane.A * rPlane.A +
		rPlane.B * rPlane.B +
		rPlane.C * rPlane.C);
	rPlane.A /= length;
	rPlane.B /= length;
	rPlane.C /= length;
	rPlane.D /= length;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool Candy::Scene::Camera::ItlInFrustum(glm::vec3 const &oCenter, const double dRadius)
{
	GLfloat dist;
	for (int i = 0; i < 6; ++i)
	{
		dist = frustum.planes[i].A * oCenter.x +
			frustum.planes[i].B * oCenter.y +
			frustum.planes[i].C * oCenter.z +
			frustum.planes[i].D;
		if (dist <= -dRadius)
			return false;
	}
	nNumShownObjects++;
	return true;
}

const glm::vec3 Candy::Scene::Camera::getDirection()
{
  return v3Direction;
}