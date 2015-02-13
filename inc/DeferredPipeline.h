#pragma once
#include <GL/glew.h>

#include <memory>

#include <GeometryBuffer.h>
#include <VertexFormat.h>
#include <GL/GLProgram.h>
#include <glm/glm.hpp>
#include <GL/GLCubeMap.h>
#include <Scene/Pointlight.h>

void IntInitializeGeometryPass();

namespace Candy
{
	
  class DeferredPipeline
  {
  public:

	//types
	enum{
		POST_TEXTURE_READ = 0,
		POST_TEXTURE_WRITE,
		POST_TEXTURE_NUM	//Number of postprocessing textures
	};

	enum{
		POST_FBO_READ = 0,
		POST_FBO_WRITE,
		POST_FBO_NUM
	};

	DeferredPipeline();
	virtual ~DeferredPipeline();
  void initialize(int fbuf_width, int fbuf_height);
  void beginGeometryPass();
  void endGeometryPass();
	void drawCubeMap(glm::mat4& m4ViewMatrix, glm::mat4& m4ProjectionMatrix);
  void beginLightingPass();
  void runPostprocess(RenderDevice &device);
	void runScreenSpaceAmbientOcclusion(RenderDevice &device);
	void runBloom(RenderDevice &device);
  void displayColorbuffer();
  void endLightingPass(RenderDevice* device);
  void bindTextures();
  void setMatrices(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
  void beginShadowPass(Candy::Scene::Pointlight& light, unsigned direction, const  glm::mat4& test, const glm::mat4& test2);
  void endShadowPass(RenderDevice* device, const Candy::Scene::Pointlight& light, unsigned direction);

	Candy::GL::GLProgram* GetPassthroughShader() const { return m_PassthroughShader; };
  Candy::GL::GLProgram* GetPassthroughShaderAnimated() const { return m_PassthroughShaderAnimated; };
  Candy::GL::GLProgram* GetShadowpassShader() const { return m_shadowmapShader; };
  Candy::GL::GLProgram* GetShadowpassShaderAnimated() const { return m_shadowmapShaderAnimated; };
  void ItlInitializeBloom();
  private:
	void ItlInitializeGeometryPass();
  void ItlInitializeShadowPass();
	void ItlInitializeLightningPass();

	void ItlInitializeCubeMapPass();

	void ItlInitializeSSAO();
	void ItlSwapFrameBuffers(GLuint fbo[POST_FBO_NUM], GLuint tex[POST_TEXTURE_NUM]);


  GLuint m_fbo;
  GLenum DrawBuffers[5]; // GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4
  GLuint m_textures[5];  // world, diffuse, normal, spec, alpha
  GLuint m_depthTexture; // texture for depth information
	GLuint m_postFbo[POST_FBO_NUM];
	GLuint m_bloom_postFbo[POST_FBO_NUM];
	GLuint m_color[POST_TEXTURE_NUM]; // postprocessing buffers
	GLuint m_bloom_color[POST_TEXTURE_NUM]; // postprocessing buffers
	GLuint m_screenshot; //screenshot of the scene (for postprocessing)

  Candy::GeometryBuffer* fboQuadGeo;
  Candy::VertexFormat* fboVertexFormat;
  Candy::GL::GLProgram* m_PassthroughShader;
  Candy::GL::GLProgram* m_PassthroughShaderAnimated;
  Candy::GL::GLProgram* m_shadowmapShader;
  Candy::GL::GLProgram* m_shadowmapShaderAnimated;
	Candy::GL::GLProgram m_CubemapShader;
	Candy::GL::GLProgram m_bloom_post;
	Candy::GL::GLProgram m_bloom_highpass_post;
	Candy::GL::GLProgram m_bloom_x_post;
	Candy::GL::GLProgram m_bloom_y_post;
	Candy::GL::GLProgram m_ssao_post;
	std::shared_ptr<Candy::GL::GLCubeMap> m_pCubeMapTexture;
   int width, height;
  };
}