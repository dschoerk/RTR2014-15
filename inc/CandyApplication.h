#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "RenderDevice.h"

namespace Candy
{
  class CandyApplication
  {
  public:
    CandyApplication();
    virtual ~CandyApplication();
    //  explicit CandyApplication();
    virtual void update() {};
    virtual void render() {};
    virtual void initialize() {};
    void run();
    double getFrameTime();
    double getAbosulteTime();
    double getFramesPerSecond();
    unsigned int getFrameNumber();

    RenderDevice* getContext();

  private:
    double absoluteTime, frameTime;
    unsigned int frameNumber;
    RenderDevice* pRD;
  };
}