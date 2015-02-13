#include "Scene/SceneObject.h"
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/spline.hpp>
#include "RenderDevice.h"

#include <algorithm>

struct Vertex2
{
	glm::vec3 position;
	glm::vec3 color;
};

Candy::Scene::SceneObject::SceneObject() : m4WorldMatrix(glm::mat4(1))
{

}

Candy::Scene::SceneObject::SceneObject(const glm::mat4& _m4WorldMatrix) : m4WorldMatrix(_m4WorldMatrix)
{

}

Candy::Scene::SceneObject::~SceneObject()
{

}

void Candy::Scene::SceneObject::update(double dTime){
  setWorldMatrix(catmulRom(scene_path.positions, scene_path.directions, dTime));
}

const glm::vec3 Candy::Scene::SceneObject::getPosition()
{
	auto worldMat = glm::inverse(getWorldMatrix());
	return glm::vec3(worldMat * glm::vec4(0, 0, 0, 1));
}

void Candy::Scene::SceneObject::cubic_hermite(float p0, float p1, float m0, float m1, float t, float& value, float& tangent)
{
	m0 *= 1000.0f;
	m1 *= 1000.0f;

	float a = 2.f * p0 - 2.f * p1 + m0 + m1;
	float b = -3.f*p0 + 3.f*p1 - 2.f*m0 - m1;
	float c = m0;
	float d = p0;

	value = a*t*t*t + b*t*t + c*t + d;
	tangent = a*3.f*t*t + b*2.f*t + c;
}

glm::mat4 Candy::Scene::SceneObject::hermiteSpline(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& directions, float t)
{
	t = fmodf(t, static_cast<float>(points.size()));
	int idx0 = static_cast<int>(t);
	int idx1 = (idx0 + 1) % points.size();
	float section = t - idx0;

	float x, t_x, y, t_y, z, t_z;
	cubic_hermite(points[idx0][0], points[idx1][0], -directions[idx0][0], -directions[idx1][0], section, x, t_x);
	cubic_hermite(points[idx0][1], points[idx1][1], -directions[idx0][1], -directions[idx1][1], section, y, t_y);
	cubic_hermite(points[idx0][2], points[idx1][2], -directions[idx0][2], -directions[idx1][2], section, z, t_z);

	/*x = points[idx0][0];
	y = points[idx0][1];
	z = points[idx0][2];
	*/
	/*t_x = directions[idx0][0];
	t_y = directions[idx0][1];
	t_z = directions[idx0][2];*/

	/* glm::vec3 translation(x, y, z);
	glm::vec3 tangent(t_x, t_y, t_z);
	tangent = glm::normalize(tangent);
	glm::vec3 bitangent = glm::cross(glm::vec3(0, 1, 0), tangent);
	glm::vec3 normal = glm::cross(tangent, bitangent);



	//rotation = glm::transpose(rotation);

	//return m4ViewMatrix;
	return glm::translate(glm::mat4(1), translation) * rotation;*/

	glm::vec3 translation(x, y, z);
	glm::vec3 tangent(t_x, t_y, t_z);


	//std::cout << "tangent " << t_x << " " << t_y << " " << t_z << std::endl;

	tangent = glm::normalize(tangent);
	glm::vec3 bitangent = glm::normalize(glm::cross(glm::vec3(0, 1, 0), tangent));
	glm::vec3 normal = glm::normalize(glm::cross(tangent, bitangent));

	/*glm::mat4 rotation(
	bitangent[0], bitangent[1], bitangent[2], 0,
	normal[0], normal[1], normal[2], 0,
	tangent[0], tangent[1], tangent[2], 0,
	0, 0, 0, 1);*/
	glm::mat4 rotation(
		bitangent[0], normal[0], tangent[0], 0,
		bitangent[1], normal[1], tangent[1], 0,
		bitangent[2], normal[2], tangent[2], 0,
		translation[0], translation[1], translation[2], 1);

	//rotation = glm::(rotation);

	auto m4ViewMatrix = glm::lookAt(
		translation,			// Camera is here
		translation + tangent,		// and looks here : at the same position, plus "direction"
		glm::vec3(0, 1, 0)                // Head is up
		);

	return  m4ViewMatrix;/*glm::translate(glm::mat4(1), translation) * *///rotation;

}

glm::vec3 Candy::Scene::SceneObject::hermiteSplinePosition(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& directions, float t)
{
	t = fmodf(t, static_cast<float>(points.size()));
	int idx0 = static_cast<int>(t);
	int idx1 = (idx0 + 1) % points.size();
	float section = t - idx0;

	float x, t_x, y, t_y, z, t_z;
	cubic_hermite(points[idx0][0], points[idx1][0], -directions[idx0][0], -directions[idx1][0], section, x, t_x);
	cubic_hermite(points[idx0][1], points[idx1][1], -directions[idx0][1], -directions[idx1][1], section, y, t_y);
	cubic_hermite(points[idx0][2], points[idx1][2], -directions[idx0][2], -directions[idx1][2], section, z, t_z);

	return glm::vec3(x, y, z);

}

glm::mat4 Candy::Scene::SceneObject::catmulRom(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& directions, float t)
{
  t = fmodf(t, static_cast<float>(points.size()));

  int idx0 = static_cast<int>(t);
  int idx1 = std::min<int>(idx0 + 1, points.size() - 1);
  int pre = std::max(idx0 - 1, 0);
  int post = std::min<int>(idx1 + 1, points.size() - 1);
  float p = t - idx0;

  //std::cout << pre << " " << idx0 << " " << idx1 << " " << post << " " << std::endl;
  //std::cout << points.size() << std::endl;

  glm::vec3 pos = glm::catmullRom(points[pre], points[idx0], points[idx1], points[post], p);
  glm::vec3 interp_dir = glm::catmullRom(directions[pre], directions[idx0], directions[idx1], directions[post], p);

  glm::vec3 fwd = -glm::normalize(interp_dir);
  glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), fwd));
  glm::vec3 up = glm::normalize(glm::cross(fwd, right));

  glm::mat3 transform(
    right.x, up.x, fwd.x,
    right.y, up.y, fwd.y,
    right.z, up.z, fwd.z);

  return((glm::translate(glm::mat4(1), pos) * glm::mat4(glm::transpose(transform))));
}

glm::vec3 Candy::Scene::SceneObject::catmulRomPositionOnly(const std::vector<glm::vec3>& points, float t)
{
  t = fmodf(t, static_cast<float>(points.size()));

  int idx0 = static_cast<int>(t);
  int idx1 = std::min<int>(idx0 + 1, points.size() - 1);
  int pre = std::max(idx0 - 1, 0);
  int post = std::min<int>(idx1 + 1, points.size() - 1);
  float p = t - idx0;

  //std::cout << pre << " " << idx0 << " " << idx1 << " " << post << " " << std::endl;
  //std::cout << points.size() << std::endl;

  glm::vec3 pos = glm::catmullRom(points[pre], points[idx0], points[idx1], points[post], p);
  return pos;
}

void Candy::Scene::SceneObject::addNodeToPath(const glm::vec3& position, const glm::vec3& direction)
{
	scene_path.positions.push_back(position);
	scene_path.directions.push_back(direction);
}

void Candy::Scene::SceneObject::addLerpToPath(int n)
{
  if (n < 1)
    return;

  glm::vec3 p1 = scene_path.positions[scene_path.positions.size() - 2];
  glm::vec3 p2 = scene_path.positions[scene_path.positions.size() - 1];

  scene_path.positions.pop_back();
  for (int i = 0; i <= n;i++)
  {
    scene_path.positions.push_back(glm::mix(p1, p2, (float)i / (float)n));
  }
}

glm::vec3 Candy::Scene::SceneObject::getLastPathPosition()
{
  return scene_path.positions.back();
}

void Candy::Scene::SceneObject::clearPath()
{
  scene_path.positions.clear();
  scene_path.directions.clear();

}

void Candy::Scene::SceneObject::genTrail()
{
	simpleVFormat = new Candy::VertexFormat();
	m_trail = new Candy::GeometryBuffer();

	std::vector<Vertex2> verts;
	simpleVFormat->addVertexElement(0, Candy::FLOAT_VEC3, 0, Candy::POSITION);
	simpleVFormat->addVertexElement(1, Candy::FLOAT_VEC3, 12, Candy::COLOR);

	const int points_per_segment = 1000;
	for (int i = 0; i < scene_path.positions.size()*points_per_segment; i++)
	{
		auto transform = catmulRom(scene_path.positions, scene_path.directions, (float)i / (float)points_per_segment);
		auto p = transform * glm::vec4(0, 0, 0, 1.f);
		Vertex2 v = { glm::vec3(p), glm::vec3(1, 0, 0) };
		verts.push_back(v);

		//std::cout << p[0] << " " << p[1] << " " << p[2] << std::endl;
	}

	m_trail->upload(verts);
}

void Candy::Scene::SceneObject::drawTrail(Candy::RenderDevice* pRD)
{
	pRD->draw(m_trail, simpleVFormat, Candy::PrimitiveMode::LINES_STRIP);
}
