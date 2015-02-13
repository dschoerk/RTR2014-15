#pragma once

/**
* H file for the bounding box
* @author David Pfahler
*/

// Library Includes
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace Candy
{
	namespace Scene
	{
		class BoundingBox
		{
		public:
			BoundingBox();
			BoundingBox(glm::vec3& v3Min, glm::vec3& v3Max);
			virtual ~BoundingBox();

			void SetBoundingBox(glm::vec3& v3Min, glm::vec3& v3Max);

			glm::vec3 halfwith;
			glm::vec3 position;
			double radius;
		};
	}
}
