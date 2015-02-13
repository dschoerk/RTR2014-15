#pragma once

#include <GeometryBuffer.h>
#include <Scene/BoundingBox.h> //is a bounding box

namespace Candy
{
  class Geometry : public Scene::BoundingBox
  {
    friend class RenderDevice;
  public:

	// Constructor

    Geometry(GeometryBuffer* _buf, 
             int _startIndex, 
             int _primitiveNumber,
			 glm::vec3 _v3Min,
			 glm::vec3 _v3Max) : Scene::BoundingBox(_v3Min,_v3Max),
									buffer(_buf), 
									startIndex(_startIndex), 
									primitiveNumber(_primitiveNumber)
    {}

  private:
    GeometryBuffer* buffer;
    int startIndex;
    int primitiveNumber;
	
  };
}