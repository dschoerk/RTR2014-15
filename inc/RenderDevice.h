#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

#include <Datatypes.h>
#include <PrimitiveBuffer.h>
#include <Mesh.h>

#include <typeinfo>
#include <VertexFormat.h>
#include <GeometryBuffer.h>
#include <Geometry.h>
#include <Scene/Camera.h>
#include <GL/PrimitiveMode.h>


namespace Candy
{
  class RenderDevice
  {
  public:
	RenderDevice();
	virtual ~RenderDevice();

    bool createRenderingContext(int window_width, int window_height, bool fullscreen, const char* window_name, int oglMinor, int oglMajor);
    void swapBuffers();
    void setClearColor(const glm::vec4 &color);
    void draw(GeometryBuffer* buffer, VertexFormat* format, PrimitiveMode mode = PrimitiveMode::TRIANGLES);
    void draw(Geometry* buffer, VertexFormat* format, PrimitiveMode mode = PrimitiveMode::TRIANGLES);
    void setClipboardString(const std::string& str);

	Scene::Camera* getCamera() const { return pCamera; };

	// Returns if the camera is currently free moveable or not
	bool IsCameraFreeMoveable();
	bool IsBloomOn();
	bool IsSSAOOn();
  bool IsAction1Pressed();

	bool ShouldClose();

	bool keydown(int key);
  bool keyup(int key);
    /*template<typename T>
    void createPrimitiveBuffer(const std::vector<T>& vertices, const VertexFormat& format, PrimitiveBuffer& ret_pbuffer);
    template <class T, class I>
    void createPrimitiveBuffer(const std::vector<T>& vertices, const std::vector<I>& indices, VertexFormat const& format, PrimitiveBuffer& ret_pbuffer);*/
    //template <class T, class I>
    //void createPrimitiveBuffer(const Mesh& mesh, VertexFormat const& format, FieldType indexType, PrimitiveBuffer& ret_pbuffer);
    
    // Draw Commands
    void drawPrimitives(PrimitiveBuffer& buffer, PrimitiveMode mode = PrimitiveMode::TRIANGLES);

  private:

	static void error_callback(int error, const char* description);
	static void key_callback(GLFWwindow* pWindow, int iKey, int iScancode, int iAction, int iMods);

    GLFWwindow* wnd;
	Scene::Camera* pCamera;
  };

  /*template <typename T>
  void RenderDevice::createPrimitiveBuffer(const std::vector<T>& vertices, VertexFormat const& format, PrimitiveBuffer& ret_pbuffer)
  {
    unsigned int formatByteSize = 0;
    for (auto e : format)
    {
      formatByteSize += byteSize(e.fieldType);
    }

    ret_pbuffer.bind();
    ret_pbuffer.setNumElements(vertices.size());

    ret_pbuffer.initVBuffer();
    ret_pbuffer.getVbuffer()->upload(vertices);
    ret_pbuffer.getVbuffer()->bind();

    assert(vertices.size() * sizeof(T) % formatByteSize == 0);

    for (auto e : format)
    {
      glEnableVertexAttribArray(e.usage);
      glVertexAttribPointer(e.usage, components(e.fieldType), basetype(e.fieldType), GL_FALSE, formatByteSize, reinterpret_cast<void*>(e.byteOffset));
    }
    
    ret_pbuffer.unbind();
  }

  template <typename T, typename I>
  void RenderDevice::createPrimitiveBuffer(const std::vector<T>& vertices, const std::vector<I>& indices, VertexFormat const& format, PrimitiveBuffer& ret_pbuffer)
  {
    GLuint indexType;
    if (typeid(I) == typeid(unsigned int))
      indexType = GL_UNSIGNED_INT;
    if (typeid(I) == typeid(unsigned short))
      indexType = GL_UNSIGNED_SHORT;
    else
      assert(false && "The index type is not supported");

    unsigned int formatByteSize = 0;
    for (auto e : format)
    {
      formatByteSize += byteSize(e.fieldType);
    }

    ret_pbuffer.bind();
    ret_pbuffer.setNumElements(indices.size());
    ret_pbuffer.setIndexType(indexType);

    ret_pbuffer.initVBuffer();
    ret_pbuffer.getVbuffer()->upload(vertices);
    ret_pbuffer.getVbuffer()->bind();

    ret_pbuffer.initIBuffer();
    ret_pbuffer.getIbuffer()->upload(indices);
    ret_pbuffer.getIbuffer()->bind();

    assert(vertices.size() * sizeof(T) % formatByteSize == 0);

    for (auto e : format)
    {
      glEnableVertexAttribArray(e.usage);
      glVertexAttribPointer(e.usage, components(e.fieldType), basetype(e.fieldType), GL_FALSE, formatByteSize, reinterpret_cast<void*>(e.byteOffset));
    }

    ret_pbuffer.unbind();
  }*/
}