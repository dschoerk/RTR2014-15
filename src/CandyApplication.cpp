#include <CandyApplication.h>

#include <iostream> // remove me asap

Candy::CandyApplication::CandyApplication()
{
	pRD = new RenderDevice();
}

Candy::CandyApplication::~CandyApplication()
{
	if (pRD != nullptr)
	{
		delete pRD;
		pRD = nullptr;
	}
}

void Candy::CandyApplication::run()
{
  initialize();

//  glfwSetInputMode(wnd, GLFW_STICKY_KEYS, GL_TRUE);
  glfwSetTime(0);
  double lastTimestep = 0;
  frameNumber = 0;
  do
  {
    absoluteTime = glfwGetTime();
    frameTime = absoluteTime - lastTimestep;
    lastTimestep = absoluteTime;

    update();
    render();
    pRD->swapBuffers();
    glfwPollEvents();

    frameNumber++;
  } while (pRD->ShouldClose());
}

double Candy::CandyApplication::getFrameTime()
{
  return frameTime;
}

double Candy::CandyApplication::getAbosulteTime()
{
  return absoluteTime;
}

double Candy::CandyApplication::getFramesPerSecond()
{
  return 1.f / frameTime;
}

unsigned Candy::CandyApplication::getFrameNumber()
{
  return frameNumber;
}

Candy::RenderDevice* Candy::CandyApplication::getContext()
{
  return pRD;
}
