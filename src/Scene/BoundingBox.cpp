#include "Scene/BoundingBox.h"

Candy::Scene::BoundingBox::BoundingBox(glm::vec3& v3Min, glm::vec3& v3Max)
{
	SetBoundingBox(v3Min, v3Max);
}

Candy::Scene::BoundingBox::BoundingBox()
{
	halfwith = position = glm::vec3(0);
	radius = 0.f;
}

Candy::Scene::BoundingBox::~BoundingBox()
{}



void Candy::Scene::BoundingBox::SetBoundingBox(glm::vec3& v3Min, glm::vec3& v3Max)
{
	halfwith	= glm::abs(v3Min - v3Max) / 2.0f;
	position	= v3Min + halfwith;
	radius		= glm::length(halfwith);
}
