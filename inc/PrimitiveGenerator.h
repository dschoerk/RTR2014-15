#pragma once

#include <vector>
#include <glm/glm.hpp>

#include <Mesh.h>

using namespace std;
using namespace glm;

namespace Candy
{
  namespace PrimitiveGeometry
  {
    template<typename T, typename I>
    void icosaeder(IndexedMesh<T, I>& m, float diameter = 1.0f)
    {
      m.vertices.resize(12);
      m.indices.clear();

      double X = .525731112119133606*diameter;
      double Z = .850650808352039932*diameter;

      m.vertices[0].position = vec3(-X, 0.0, Z);
      m.vertices[1].position = vec3(X, 0.0, Z);
      m.vertices[2].position = vec3(-X, 0.0, -Z);
      m.vertices[3].position = vec3(X, 0.0, -Z);
      m.vertices[4].position = vec3(0.0, Z, X);
      m.vertices[5].position = vec3(0.0, Z, -X);
      m.vertices[6].position = vec3(0.0, -Z, X);
      m.vertices[7].position = vec3(0.0, -Z, -X);
      m.vertices[8].position = vec3(Z, X, 0.0);
      m.vertices[9].position = vec3(-Z, X, 0.0);
      m.vertices[10].position = vec3(Z, -X, 0.0);
      m.vertices[11].position = vec3(-Z, -X, 0.0);

      m.indices.push_back(0);
      m.indices.push_back(4);
      m.indices.push_back(1);

      m.indices.push_back(0);
      m.indices.push_back(9);
      m.indices.push_back(4);

      m.indices.push_back(9);
      m.indices.push_back(5);
      m.indices.push_back(4);

      m.indices.push_back(4);
      m.indices.push_back(5);
      m.indices.push_back(8);

      m.indices.push_back(4);
      m.indices.push_back(8);
      m.indices.push_back(1);

      m.indices.push_back(8);
      m.indices.push_back(10);
      m.indices.push_back(1);

      m.indices.push_back(8);
      m.indices.push_back(3);
      m.indices.push_back(10);

      m.indices.push_back(5);
      m.indices.push_back(3);
      m.indices.push_back(8);

      m.indices.push_back(5);
      m.indices.push_back(2);
      m.indices.push_back(3);

      m.indices.push_back(2);
      m.indices.push_back(7);
      m.indices.push_back(3);

      m.indices.push_back(7);
      m.indices.push_back(10);
      m.indices.push_back(3);

      m.indices.push_back(7);
      m.indices.push_back(6);
      m.indices.push_back(10);

      m.indices.push_back(7);
      m.indices.push_back(11);
      m.indices.push_back(6);

      m.indices.push_back(11);
      m.indices.push_back(0);
      m.indices.push_back(6);

      m.indices.push_back(0);
      m.indices.push_back(1);
      m.indices.push_back(6);

      m.indices.push_back(6);
      m.indices.push_back(1);
      m.indices.push_back(10);

      m.indices.push_back(9);
      m.indices.push_back(0);
      m.indices.push_back(11);

      m.indices.push_back(9);
      m.indices.push_back(11);
      m.indices.push_back(2);

      m.indices.push_back(9);
      m.indices.push_back(2);
      m.indices.push_back(5);

      m.indices.push_back(7);
      m.indices.push_back(2);
      m.indices.push_back(11);
    }

    template<typename T>
    T halfVector(const T& a, const T& b, float diameter)
    {
      T mid;
      mid.position = a.position + ((b.position - a.position) * 0.5f);
      mid.position = normalize(mid.position)*diameter;
      return mid;
    }

    template<typename T, typename I>
    void subdivide(uint firstIndexIndex,
      vector<I>& indexBuffer,
      vector<T>& vertexBuffer, uint divisionLevel, float diameter)
    {
      const auto indexA = indexBuffer[firstIndexIndex + 0];
      const auto indexB = indexBuffer[firstIndexIndex + 1];
      const auto indexC = indexBuffer[firstIndexIndex + 2];

      const auto indexAB = vertexBuffer.size() + 0;
      const auto indexBC = vertexBuffer.size() + 1;
      const auto indexAC = vertexBuffer.size() + 2;

      const auto a = vertexBuffer[indexA];
      const auto b = vertexBuffer[indexB];
      const auto c = vertexBuffer[indexC];

      unsigned idxv = vertexBuffer.size();
      vertexBuffer.resize(vertexBuffer.size() + 3);
      vertexBuffer[idxv + 0] = halfVector(a, b, diameter);
      vertexBuffer[idxv + 1] = halfVector(b, c, diameter);
      vertexBuffer[idxv + 2] = halfVector(c, a, diameter);

      //Erster Index vom alten Dreieck bleibt gleich
      indexBuffer[firstIndexIndex + 1] = indexAB;
      indexBuffer[firstIndexIndex + 2] = indexAC;

      unsigned idxi = indexBuffer.size();
      indexBuffer.resize(indexBuffer.size() + 9);

      const uint indexIndex1 = indexBuffer.size();
      indexBuffer[idxi + 0] = indexAB;
      indexBuffer[idxi + 1] = indexB;
      indexBuffer[idxi + 2] = indexBC;

      const uint indexIndex2 = indexBuffer.size();
      indexBuffer[idxi + 3] = indexBC;
      indexBuffer[idxi + 4] = indexC;
      indexBuffer[idxi + 5] = indexAC;

      const uint indexIndex3 = indexBuffer.size();
      indexBuffer[idxi + 6] = indexAB;
      indexBuffer[idxi + 7] = indexBC;
      indexBuffer[idxi + 8] = indexAC;

      divisionLevel--;
      if (divisionLevel > 0)
      {
        subdivide<T, I>(firstIndexIndex, indexBuffer, vertexBuffer, divisionLevel, diameter);
        subdivide<T, I>(indexIndex1, indexBuffer, vertexBuffer, divisionLevel, diameter);
        subdivide<T, I>(indexIndex2, indexBuffer, vertexBuffer, divisionLevel, diameter);
        subdivide<T, I>(indexIndex3, indexBuffer, vertexBuffer, divisionLevel, diameter);
      }
    }

    template<typename T, typename I>
    void sphere(IndexedMesh<T, I>& m, float diameter = 1.0f, unsigned int div = 1)
    {
      Candy::PrimitiveGeometry::icosaeder(m, diameter);
      for (int i = 0; div > 0 && i < 20; i++)
      {
        subdivide(3 * i, m.indices, m.vertices, div, diameter);
      }
      //subdivide(0, m.indices, m.vertices, 1, diameter);

      /*	m.normals.resize(m.positions.size(),vec3(0));
      for(unsigned int i=0;i<m.positions.size();i++)
      {
      m.normals[i] = normalize(m.positions[i]);
      }*/
    }

    template<typename T, typename I>
    void cube(IndexedMesh<T, I>& m, vec3 min = vec3(-1, -1, -1), vec3 max = vec3(1, 1, 1))
    {
      m.vertices.resize(6*4);

      m.vertices[0].position = max; //front
      m.vertices[1].position = vec3(min.x, max.y, max.z);
      m.vertices[2].position = vec3(min.x, min.y, max.z);
      m.vertices[3].position = vec3(max.x, min.y, max.z);

      /*m.vertices[0].normal = vec3(0, 0, 1);
      m.vertices[1].normal = vec3(0, 0, 1);
      m.vertices[2].normal = vec3(0, 0, 1);
      m.vertices[3].normal = vec3(0, 0, 1);*/

      /*	m.texcoords[0].position = vec2(0.5,1-0.5));
      m.texcoords[0].position = vec2(0.25,1-0.5));
      m.texcoords[0].position = vec2(0.25,1-1));
      m.texcoords[0].position = vec2(0.5,1-1));*/

      m.vertices[4].position = max; //right
      m.vertices[5].position = vec3(max.x, min.y, max.z);
      m.vertices[6].position = vec3(max.x, min.y, min.z);
      m.vertices[7].position = vec3(max.x, max.y, min.z);

      /*m.vertices[4].normal = vec3(1, 0, 0);
      m.vertices[5].normal = vec3(1, 0, 0);
      m.vertices[6].normal = vec3(1, 0, 0);
      m.vertices[7].normal = vec3(1, 0, 0);*/

      /*	m.texcoords[0].position = vec2(0.5,1-0.5));
      m.texcoords[0].position = vec2(0.5,1-1));
      m.texcoords[0].position = vec2(0.75,1-1));
      m.texcoords[0].position = vec2(0.75,1-0.5));*/

      m.vertices[8].position = max; //top
      m.vertices[9].position = vec3(max.x, max.y, min.z);
      m.vertices[10].position = vec3(min.x, max.y, min.z);
      m.vertices[11].position = vec3(min.x, max.y, max.z);

      /*m.vertices[8].normal = vec3(0, 1, 0);
      m.vertices[9].normal = vec3(0, 1, 0);
      m.vertices[10].normal = vec3(0, 1, 0);
      m.vertices[11].normal = vec3(0, 1, 0);*/

      /*		m.texcoords[0].position = vec2(0.5,0.5));
      m.texcoords[0].position = vec2(0.5,1));
      m.texcoords[0].position = vec2(0.25,1));
      m.texcoords[0].position = vec2(0.25,0.5));*/


      m.vertices[12].position = vec3(min.x, max.y, max.z); //left
      m.vertices[13].position = vec3(min.x, max.y, min.z);
      m.vertices[14].position = min;
      m.vertices[15].position = vec3(min.x, min.y, max.z);

      /*m.vertices[12].normal = vec3(-1, 0, 0);
      m.vertices[13].normal = vec3(-1, 0, 0);
      m.vertices[14].normal = vec3(-1, 0, 0);
      m.vertices[15].normal = vec3(-1, 0, 0);*/

      /*	m.texcoords[0].position = vec2(0.25,1-0.5));
      m.texcoords[0].position = vec2(0,1-0.5));
      m.texcoords[0].position = vec2(0,1-1));
      m.texcoords[0].position = vec2(0.25,1-1));*/

      m.vertices[16].position = min; //bottom
      m.vertices[17].position = vec3(min.x, min.y, min.z);
      m.vertices[18].position = vec3(max.x, min.y, max.z);
      m.vertices[19].position = vec3(min.x, min.y, max.z);

      /*m.vertices[16].normal = vec3(0, -1, 0);
      m.vertices[17].normal = vec3(0, -1, 0);
      m.vertices[18].normal = vec3(0, -1, 0);
      m.vertices[19].normal = vec3(0, -1, 0);*/

      /*	m.texcoords[0].position = vec2());
      m.texcoords[0].position = vec2());
      m.texcoords[0].position = vec2());
      m.texcoords[0].position = vec2());*/

      m.vertices[20].position = vec3(max.x, min.y, min.z); //back
      m.vertices[21].position = min;
      m.vertices[22].position = vec3(min.x, max.y, min.z);
      m.vertices[23].position = vec3(max.x, max.y, min.z);

      /*m.vertices[20].normal = vec3(0, 0, -1);
      m.vertices[21].normal = vec3(0, 0, -1);
      m.vertices[22].normal = vec3(0, 0, -1);
      m.vertices[23].normal = vec3(0, 0, -1);*/

      /*	m.texcoords[0].position = vec2(0.75,1-1));
      m.texcoords[0].position = vec2(1,1-1));
      m.texcoords[0].position = vec2(1,1-0.5));
      m.texcoords[0].position = vec2(0.75,1-0.5));*/

      //front
      m.indices.push_back(0);
      m.indices.push_back(1);
      m.indices.push_back(2);

      m.indices.push_back(2);
      m.indices.push_back(3);
      m.indices.push_back(0);

      //right
      m.indices.push_back(4);
      m.indices.push_back(5);
      m.indices.push_back(6);

      m.indices.push_back(6);
      m.indices.push_back(7);
      m.indices.push_back(4);

      //top
      m.indices.push_back(8);
      m.indices.push_back(9);
      m.indices.push_back(10);

      m.indices.push_back(10);
      m.indices.push_back(11);
      m.indices.push_back(8);

      //left
      m.indices.push_back(12);
      m.indices.push_back(13);
      m.indices.push_back(14);

      m.indices.push_back(14);
      m.indices.push_back(15);
      m.indices.push_back(12);

      //bottom
      m.indices.push_back(16);
      m.indices.push_back(18);
      m.indices.push_back(17);

      m.indices.push_back(18);
      m.indices.push_back(19);
      m.indices.push_back(16);

      //back
      m.indices.push_back(20);
      m.indices.push_back(21);
      m.indices.push_back(22);

      m.indices.push_back(22);
      m.indices.push_back(23);
      m.indices.push_back(20);
      
    }

    void cylinder(IndexedMesh<glm::vec3, unsigned>& m, float radius, float height, int div = 8);
  };

}