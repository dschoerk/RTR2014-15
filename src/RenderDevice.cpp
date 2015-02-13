#include <RenderDevice.h>
#include <iostream> // remove me

#include "utils.h"

bool bIsCameraFreeMovebale = false;
bool bIsBloomOn = true;
bool bIsSSAOOn = true;
bool bIsAction1Pressed;

void Candy::RenderDevice::error_callback(int error, const char* description)
{
  std::cout << description << std::endl;
}

void Candy::RenderDevice::key_callback(GLFWwindow* pWindow, int iKey, int iScancode, int iAction, int iMods)
{
	if (iKey == GLFW_KEY_ESCAPE && iAction == GLFW_PRESS)
	{
		std::cout << "Escape Pressed: Window will close" << std::endl;
		glfwSetWindowShouldClose(pWindow, GL_TRUE);
	}

	if (iKey == GLFW_KEY_F5 && iAction == GLFW_PRESS)
	{
		std::cout << "Reload Shaders" << std::endl;
	}

	// free move able camera on/off
	if (iKey == GLFW_KEY_F3 && iAction == GLFW_PRESS)
	{
		bIsCameraFreeMovebale = !bIsCameraFreeMovebale;
		std::cout << "Set Moveable Camera to " <<
			(bIsCameraFreeMovebale ? "ON" : "OFF")
			<< std::endl;
	}

	// bloom on/off
	if (iKey == GLFW_KEY_F4 && iAction == GLFW_PRESS)
	{
		bIsBloomOn = !bIsBloomOn;
		std::cout << "Set Bloom to " <<
			(bIsBloomOn ? "ON" : "OFF")
			<< std::endl;
	}

	// ssao on/off
	if (iKey == GLFW_KEY_F6 && iAction == GLFW_PRESS)
	{
		bIsSSAOOn = !bIsSSAOOn;
		std::cout << "Set SSAO to " <<
			(bIsSSAOOn ? "ON" : "OFF")
			<< std::endl;
	}

  /*if (iKey == GLFW_KEY_SPACE && iAction == GLFW_PRESS)
  {
    bIsAction1Pressed = !bIsAction1Pressed;
  }*/
}

Candy::RenderDevice::RenderDevice()
{
	pCamera = nullptr;
}

Candy::RenderDevice::~RenderDevice()
{
	SAFE_DELETE(pCamera);
}

bool Candy::RenderDevice::createRenderingContext(int window_width, int window_height, bool fullscreen, const char* window_name, int oglMinor, int oglMajor)
{
  glewInit();
  if (!glfwInit())
    std::cout << "could not initialize GLFW" << std::endl;

  glfwWindowHint(GLFW_SAMPLES, 2); // 4x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, oglMinor); // We want OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, oglMajor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 
  glfwWindowHint(GLFW_DECORATED, GL_TRUE);
  glfwWindowHint(GLFW_REFRESH_RATE, 0);
  // Open a window and create its OpenGL context 

  auto* primaryMonitor = glfwGetPrimaryMonitor();
  if (fullscreen)
  {
	  wnd = glfwCreateWindow(window_width, window_height, window_name, primaryMonitor, nullptr);
  }
  else
  {
	  wnd = glfwCreateWindow(window_width, window_height, window_name, nullptr, nullptr);
  }

  if (wnd == NULL){
    fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible\n");
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(wnd); // Initialize GLEW 
  glewExperimental = true; // Needed in core profile 
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return false;
  }

  glfwSetErrorCallback(error_callback);
  glfwSetKeyCallback(wnd, key_callback);
  glfwSetInputMode(wnd, GLFW_STICKY_KEYS, GL_TRUE);

  glFrontFace(GL_CCW);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);
  glClearDepth(1.0f);
  /*glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
  glClampColor(GL_CLAMP_VERTEX_COLOR, GL_FALSE);
  glClampColor(GL_CLAMP_FRAGMENT_COLOR, GL_FALSE);*/

  pCamera = new Candy::Scene::Camera(wnd, glm::vec3(0), window_width, window_height);

  return true;
}

void Candy::RenderDevice::swapBuffers()
{
  glfwSwapBuffers(wnd);
}

void Candy::RenderDevice::setClearColor(const glm::vec4& color)
{
  glClearColor(color[0], color[1], color[2], color[3]);
}

void Candy::RenderDevice::draw(GeometryBuffer* buffer, VertexFormat* format, PrimitiveMode mode)
{
  format->bind();
  format->bindVertexBuffer(buffer->vbuffer);

  if (buffer->ibuffer)
  {
    buffer->ibuffer->bind();
    glDrawElements(mode, static_cast<GLsizei>(buffer->ibuffer->getNumberOfElements()), GL_UNSIGNED_INT, nullptr);
  }
  else
	  glDrawArrays(mode, 0, static_cast<GLsizei>(buffer->vbuffer->getNumberOfElements()));
  format->unbind();
}

void Candy::RenderDevice::draw(Geometry* buffer, VertexFormat* format, PrimitiveMode mode)
{
  format->bind();
  format->bindVertexBuffer(buffer->buffer->vbuffer);

  if (buffer->buffer->ibuffer)
  {
    buffer->buffer->ibuffer->bind();
    glDrawElements(mode, buffer->primitiveNumber, GL_UNSIGNED_INT, reinterpret_cast<void*>(buffer->startIndex * 4));
  }
  else
    glDrawArrays(mode, buffer->startIndex, buffer->primitiveNumber);
  format->unbind();
}

void Candy::RenderDevice::setClipboardString(const std::string& str)
{
  glfwSetClipboardString(wnd, str.c_str());
}

bool Candy::RenderDevice::keydown(int key)
{
  return glfwGetKey(wnd, key) == GLFW_PRESS;
}

bool Candy::RenderDevice::keyup(int key)
{
  return glfwGetKey(wnd, key) == GLFW_RELEASE;
}

bool Candy::RenderDevice::IsCameraFreeMoveable()
{
	return bIsCameraFreeMovebale;
}

bool Candy::RenderDevice::IsBloomOn()
{
	return bIsBloomOn;
}

bool Candy::RenderDevice::IsSSAOOn()
{
	return bIsSSAOOn;
}

bool Candy::RenderDevice::IsAction1Pressed()
{
  return bIsAction1Pressed;
}

bool Candy::RenderDevice::ShouldClose()
{
	return glfwWindowShouldClose(wnd) == 0;
}

void Candy::RenderDevice::drawPrimitives(PrimitiveBuffer& buffer, PrimitiveMode mode)
{

  buffer.bind();
  if (buffer.getIbuffer())
    glDrawElements(mode, buffer.getNumElements(), buffer.getIndexType(), nullptr);
  else
    glDrawArrays(mode, 0, buffer.getNumElements());
  buffer.unbind();
}