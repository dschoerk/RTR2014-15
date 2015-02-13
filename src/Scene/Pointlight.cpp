#include <Scene/Pointlight.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_FORCE_RADIANS

void Candy::Scene::Pointlight::initialize(unsigned size)
{
  fboSize = size;
  glGenFramebuffers(2, m_fbo);

  // Create the depth buffer
  glGenTextures(1, &m_depth);
  glBindTexture(GL_TEXTURE_2D, m_depth);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Create the tmpTex
  glGenTextures(1, &m_tmpTex);
  glBindTexture(GL_TEXTURE_2D, m_tmpTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, size, size, 0, GL_RG, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Create the cube map
  glGenTextures(1, &m_shadowMap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowMap);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  for (unsigned i = 0; i < 6; i++)
  {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RG32F, size, size, 0, GL_RG, GL_FLOAT, nullptr);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo[0]);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth, 0);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, m_shadowMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (Status != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "FBO error, Status:" << Status << std::endl;
    return;
  }

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  mvp.push_back(glm::mat4(
    0, 0, 1, 0,
    0, 1, 0, 0,
    1, 0, 0, 0,
    0, 0, 0, 1));

  mvp.push_back(glm::mat4(
    0, 0, -1, 0,
    0, 1, 0, 0,
    -1, 0, 0, 0,
    0, 0, 0, 1));
  
  mvp.push_back(glm::mat4(
    -1, 0, 0, 0,
    0, 0, 1, 0,
    0, -1, 0, 0,
    0, 0, 0, 1));

  mvp.push_back(glm::mat4(
    -1, 0, 0, 0,
    0, 0, -1, 0,
    0, 1, 0, 0,
    0, 0, 0, 1));

  mvp.push_back(glm::mat4(
    -1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1));

  mvp.push_back(glm::mat4(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, -1, 0,
    0, 0, 0, 1));

   m4projection = glm::perspective(glm::pi<float>() / 2.0f, 1.0f, 1.0f, 3000.0f);
}

const glm::mat4 Candy::Scene::Pointlight::getMVP(PointlightDirections dir)
{
  // setup projection
	return m4projection * mvp[dir] *  glm::translate(glm::mat4(1), -position);
}

const glm::mat4 Candy::Scene::Pointlight::getMV(PointlightDirections dir)
{
	return mvp[dir] * m4WorldMatrix*  glm::translate(glm::mat4(1), -position);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool Candy::Scene::Pointlight::inFrustum(Candy::Scene::BoundingBox const &boundingBox, unsigned dir)
{
	GLfloat dist;
	for (int i = 0; i < 6; ++i)
	{
		dist = frustum.planes[i].A * boundingBox.position.x +
			frustum.planes[i].B * boundingBox.position.y +
			frustum.planes[i].C * boundingBox.position.z +
			frustum.planes[i].D;
		if (dist <= -boundingBox.radius)
			return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Candy::Scene::Pointlight::ItlExtractPlane(Plane &rPlane, int iRow)
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
void Candy::Scene::Pointlight::update(PointlightDirections dir)
{
	m4WVP = getMVP(dir);
	ItlExtractPlane(frustum.planes[0], 1);
	ItlExtractPlane(frustum.planes[1], -1);
	ItlExtractPlane(frustum.planes[2], 2);
	ItlExtractPlane(frustum.planes[3], -2);
	ItlExtractPlane(frustum.planes[4], 3);
	ItlExtractPlane(frustum.planes[5], -3);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Candy::Scene::Pointlight::updatePath(double dTime)
{
  if (hasPath())
  {
    position = catmulRomPositionOnly(scene_path.positions, dTime);
    //std::cout << position.x << " " << position.y << " " << position.z << std::endl;
  }
	//color = glm::normalize(hermiteSplinePosition(scene_path.directions, scene_path.positions, dTime));
}

glm::vec3 Candy::Scene::Pointlight::getColor()
{
	return color;
}