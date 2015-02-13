#pragma once 
#include <vector>

namespace Candy
{
  template<typename T>
  class Mesh
  {
  public:
    std::vector<T> vertices;
  };

  template<typename T, typename I>
  class IndexedMesh : public Mesh<T>
  {
  public:
    std::vector<I> indices;
  };
}