#include <DeferredPipeline.h>

#include <glm/glm.hpp>
#include <RenderDevice.h>
#include "utils.h"

#include <glm/gtc/matrix_transform.hpp> //remove

#define BLOOM_CROP 4.0f

Candy::DeferredPipeline::DeferredPipeline()
{
	m_PassthroughShader = nullptr;
  m_PassthroughShaderAnimated = nullptr;
}

Candy::DeferredPipeline::~DeferredPipeline()
{
	SAFE_DELETE(m_PassthroughShader);
  SAFE_DELETE(m_PassthroughShaderAnimated);
}

void Candy::DeferredPipeline::initialize(int fbuf_width, int fbuf_height)
{
  width = fbuf_width;
  height = fbuf_height;
  ItlInitializeGeometryPass();
  ItlInitializeShadowPass();
  ItlInitializeLightningPass();
  //ItlInitializeCubeMapPass();
  ItlInitializeBloom();
  ItlInitializeSSAO();
}

void Candy::DeferredPipeline::beginGeometryPass()
{
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
  
  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  GLuint db[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
  glDrawBuffers(5, db);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_PassthroughShader->bind(); // should be obsolet but isn't
}

void Candy::DeferredPipeline::endGeometryPass()
{
}

void Candy::DeferredPipeline::drawCubeMap(glm::mat4& m4ViewMatrix, glm::mat4& m4ProjectionMatrix)
{
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	m_CubemapShader.bind();
	m_CubemapShader.setUniform("VP", m4ProjectionMatrix*m4ViewMatrix);
	m_pCubeMapTexture->bind(0);
	glDepthMask(GL_TRUE);
}

void Candy::DeferredPipeline::beginLightingPass()
{ 
  glViewport(0, 0, width, height);
  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);

  //glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_postFbo[POST_FBO_READ]); // always draw the first, non-postprocessed image to target 0
  //glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);

  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ONE);
}

void Candy::DeferredPipeline::bindTextures()
{

	for (int i = 0; i < 5; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);
		glUniform1i(i, i);
	}

	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	glUniform1i(5, 5);
}

void Candy::DeferredPipeline::endLightingPass(RenderDevice* device)
{
	device->draw(fboQuadGeo, fboVertexFormat);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Candy::DeferredPipeline::runPostprocess(RenderDevice &device)
{  
	if (device.IsSSAOOn())
	{
		runScreenSpaceAmbientOcclusion(device);
		ItlSwapFrameBuffers(m_postFbo, m_color);
	}
	if (device.IsBloomOn())
	{
		runBloom(device);
		ItlSwapFrameBuffers(m_postFbo, m_color);
	}  
}

void Candy::DeferredPipeline::runScreenSpaceAmbientOcclusion(RenderDevice &device)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_postFbo[POST_FBO_READ]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_postFbo[POST_FBO_WRITE]);

	m_ssao_post.bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_color[POST_TEXTURE_READ]);
	m_ssao_post.setUniform("color_tex", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	m_ssao_post.setUniform("depth_tex", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_textures[2]);
	m_ssao_post.setUniform("normal_tex", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_textures[0]);
	m_ssao_post.setUniform("pos_tex", 3);

	m_ssao_post.setUniform("framebuffer_dim", glm::vec2(width, height));

	device.draw(fboQuadGeo, fboVertexFormat);
  glBindTexture(GL_TEXTURE_2D,0);
  glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void Candy::DeferredPipeline::runBloom(RenderDevice &device)
{
	glm::vec2 vec2_framebuff_dim = glm::vec2(width, height);
	// bind fbo as read / draw fbo
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_postFbo[POST_FBO_READ]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_postFbo[POST_FBO_WRITE]);

	// first copy texture
	glBindTexture(GL_TEXTURE_2D, m_screenshot);
  glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);
  

	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_bloom_postFbo[POST_FBO_READ]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_bloom_postFbo[POST_FBO_WRITE]);

	glViewport(0, 0, GLsizei(width / BLOOM_CROP), GLsizei(height / BLOOM_CROP));

	// 1) highpass the image
	m_bloom_highpass_post.bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_color[POST_TEXTURE_READ]);
	m_bloom_highpass_post.setUniform("color_tex", 0);
	m_bloom_highpass_post.setUniform("bloomCrop", BLOOM_CROP);
	m_bloom_highpass_post.setUniform("framebuffer_dim", vec2_framebuff_dim);

	device.draw(fboQuadGeo, fboVertexFormat);

	ItlSwapFrameBuffers(m_bloom_postFbo,m_bloom_color);

	// 2) blur it in x
	m_bloom_x_post.bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_bloom_color[POST_TEXTURE_READ]);
	m_bloom_x_post.setUniform("color_tex", 0);
	m_bloom_x_post.setUniform("bloomCrop", BLOOM_CROP);
	m_bloom_x_post.setUniform("framebuffer_dim", vec2_framebuff_dim);

	device.draw(fboQuadGeo, fboVertexFormat);

	ItlSwapFrameBuffers(m_bloom_postFbo, m_bloom_color);

	// 3) blur it in y
	m_bloom_y_post.bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_bloom_color[POST_TEXTURE_READ]);
	m_bloom_y_post.setUniform("color_tex", 0);
	m_bloom_y_post.setUniform("bloomCrop", BLOOM_CROP);
	m_bloom_y_post.setUniform("framebuffer_dim", vec2_framebuff_dim);

	device.draw(fboQuadGeo, fboVertexFormat);

	ItlSwapFrameBuffers(m_bloom_postFbo, m_bloom_color);

	// 4) combine the original texture with the blurred
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_postFbo[POST_FBO_WRITE]);

	m_bloom_post.bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_screenshot);
	m_bloom_post.setUniform("startTexture", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_bloom_color[POST_TEXTURE_READ]);
	m_bloom_post.setUniform("bloomTexture", 1);
	m_bloom_post.setUniform("bloomCrop", BLOOM_CROP);
	m_bloom_post.setUniform("framebuffer_dim", vec2_framebuff_dim);

	device.draw(fboQuadGeo, fboVertexFormat);
  glBindTexture(GL_TEXTURE_2D,0);
  glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void Candy::DeferredPipeline::displayColorbuffer()
{
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
//  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_postFbo[POST_FBO_READ]);
  glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Candy::DeferredPipeline::ItlSwapFrameBuffers(GLuint fbo[POST_FBO_NUM], GLuint tex[POST_TEXTURE_NUM])
{
	// swap buffers
	GLuint tmp = fbo[POST_FBO_READ];
	fbo[POST_FBO_READ] = fbo[POST_FBO_WRITE];
	fbo[POST_FBO_WRITE] = tmp;

	tmp = tex[POST_TEXTURE_READ];
	tex[POST_TEXTURE_READ] = tex[POST_TEXTURE_WRITE];
	tex[POST_TEXTURE_WRITE] = tmp;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo[POST_FBO_READ]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo[POST_FBO_WRITE]);
}
void Candy::DeferredPipeline::setMatrices(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
{
  m_PassthroughShader->setUniform("gWorld", model);
  m_PassthroughShader->setUniform("gNormal", glm::mat3(glm::transpose(glm::inverse(model))));
  m_PassthroughShader->setUniform("gWVP", projection * view * model);

  /*m_PassthroughShaderAnimated->setUniform("gWorld", model);
  m_PassthroughShaderAnimated->setUniform("gNormal", glm::mat3(glm::transpose(glm::inverse(model))));
  m_PassthroughShaderAnimated->setUniform("gWVP", projection * view * model);*/
}

void Candy::DeferredPipeline::ItlInitializeShadowPass()
{
  Candy::GL::GLShader vs("../etc/shaders/shadowpass.vs", Candy::GL::GLShaderType::VERTEX_SHADER);
  Candy::GL::GLShader fs("../etc/shaders/shadowpass.fs", Candy::GL::GLShaderType::FRAGMENT_SHADER);
  m_shadowmapShader = new Candy::GL::GLProgram();
  m_shadowmapShader->initialize();
  m_shadowmapShader->attachShader(vs);
  m_shadowmapShader->attachShader(fs);
  m_shadowmapShader->link();

  Candy::GL::GLShader vs_anim("../etc/shaders/shadowpass_animated.vs", Candy::GL::GLShaderType::VERTEX_SHADER);
  Candy::GL::GLShader fs_anim("../etc/shaders/shadowpass.fs", Candy::GL::GLShaderType::FRAGMENT_SHADER);
  m_shadowmapShaderAnimated = new Candy::GL::GLProgram();
  m_shadowmapShaderAnimated->initialize();
  m_shadowmapShaderAnimated->attachShader(vs_anim);
  m_shadowmapShaderAnimated->attachShader(fs_anim);
  m_shadowmapShaderAnimated->link();
}

void Candy::DeferredPipeline::ItlInitializeGeometryPass()
{
/*  m_PassthroughShader = new Candy::GL::GLProgram();
  Candy::GL::GLShader phong_tess_vs("../etc/shaders/phong_tess.vs", Candy::GL::GLShaderType::VERTEX_SHADER);
  Candy::GL::GLShader phong_tess_tctrl("../etc/shaders/phong_tess.tctrl", Candy::GL::GLShaderType::TESS_CONTROL_SHADER);
  Candy::GL::GLShader phong_tess_teval("../etc/shaders/phong_tess.teval", Candy::GL::GLShaderType::TESS_EVALUATION_SHADER);
  Candy::GL::GLShader phong_tess_fs("../etc/shaders/phong_tess.fs", Candy::GL::GLShaderType::FRAGMENT_SHADER);
  m_PassthroughShader->initialize();
  m_PassthroughShader->attachShader(phong_tess_vs);
  m_PassthroughShader->attachShader(phong_tess_tctrl);
  m_PassthroughShader->attachShader(phong_tess_teval);
  m_PassthroughShader->attachShader(phong_tess_fs);
  m_PassthroughShader->link();
  m_PassthroughShader->setPrimitiveMode(Candy::PrimitiveMode::PATCHES);
  */
	m_PassthroughShader = new Candy::GL::GLProgram();
	Candy::GL::GLShader vertex_shader("../etc/shaders/deferred_geometry_pass.vs", Candy::GL::GLShaderType::VERTEX_SHADER);
	Candy::GL::GLShader fragment_shader("../etc/shaders/deferred_geometry_pass.fs", Candy::GL::GLShaderType::FRAGMENT_SHADER);
	m_PassthroughShader->initialize();
	m_PassthroughShader->attachShader(vertex_shader);
	m_PassthroughShader->attachShader(fragment_shader);
	m_PassthroughShader->link();

  m_PassthroughShaderAnimated = new Candy::GL::GLProgram();
  Candy::GL::GLShader vertex_shader_anim("../etc/shaders/deferred_geometry_pass_animated.vs", Candy::GL::GLShaderType::VERTEX_SHADER);
  Candy::GL::GLShader fragment_shader_anim("../etc/shaders/deferred_geometry_pass_animated.fs", Candy::GL::GLShaderType::FRAGMENT_SHADER);
  m_PassthroughShaderAnimated->initialize();
  m_PassthroughShaderAnimated->attachShader(vertex_shader_anim);
  m_PassthroughShaderAnimated->attachShader(fragment_shader_anim);
  m_PassthroughShaderAnimated->link();

	std::vector<glm::vec2> fboQuadVertex = { { -1.f, -1.f }, { 1.f, -1.f }, { -1.f, 1.f }, { -1.f, 1.f }, { 1.f, -1.f }, { 1.f, 1.f } };
	fboQuadGeo = new Candy::GeometryBuffer;
	fboQuadGeo->upload(fboQuadVertex);

	fboVertexFormat = new Candy::VertexFormat;
	fboVertexFormat->addVertexElement(0, Candy::FLOAT_VEC2, 0, Candy::POSITION);

	glGenFramebuffers(1, &m_fbo);
	//  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

	glGenTextures(1, &m_depthTexture);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glGenTextures(5, m_textures);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
  GLint texformats[] = { GL_RGB32F, GL_RGB, GL_RGB32F, GL_RGB, GL_RGB };
	for (int i = 0; i < 5; i++)
	{
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, texformats[i], width, height, 0, GL_RGB, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, m_textures[i], 0);
	}

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTexture, 0);

	GLuint db[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4};
	glDrawBuffers(5, db);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// restore default FBO
	//  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void Candy::DeferredPipeline::ItlInitializeCubeMapPass()
{
	Candy::GL::GLShader vertex_shader("../etc/shaders/cubemap.vs", Candy::GL::GLShaderType::VERTEX_SHADER);
	Candy::GL::GLShader fragment_shader("../etc/shaders/cubemap.fs", Candy::GL::GLShaderType::FRAGMENT_SHADER);
	m_CubemapShader.initialize();
	m_CubemapShader.attachShader(vertex_shader);
	m_CubemapShader.attachShader(fragment_shader);
	m_CubemapShader.link();

	m_pCubeMapTexture = std::make_shared<Candy::GL::GLCubeMap>();
	m_pCubeMapTexture->loadFromFile("../etc/CubeMap/", false);
}

void Candy::DeferredPipeline::ItlInitializeBloom()
{

	/**
	* BLOOM Shader Stack
	*/
	Candy::GL::GLShader passthrough_vs("../etc/shaders/postprocess_passtrough.vs", Candy::GL::GLShaderType::VERTEX_SHADER);

	// the high pass
	Candy::GL::GLShader bloom_highpass_post_fs("../etc/shaders/postprocess_bloom_highpass.fs", Candy::GL::GLShaderType::FRAGMENT_SHADER);
	m_bloom_highpass_post.initialize();
	m_bloom_highpass_post.attachShader(passthrough_vs);
	m_bloom_highpass_post.attachShader(bloom_highpass_post_fs);
	m_bloom_highpass_post.link();

	// bloom for adding the blurred high pass to the standard picture
	Candy::GL::GLShader bloom_post_fs("../etc/shaders/postprocess_bloom.fs", Candy::GL::GLShaderType::FRAGMENT_SHADER);
	m_bloom_post.initialize();
	m_bloom_post.attachShader(passthrough_vs);
	m_bloom_post.attachShader(bloom_post_fs);
	m_bloom_post.link();

	// the blurring in x direction
	Candy::GL::GLShader bloom_x_post_fs("../etc/shaders/postprocess_bloom_x_blur.fs", Candy::GL::GLShaderType::FRAGMENT_SHADER);
	m_bloom_x_post.initialize();
	m_bloom_x_post.attachShader(passthrough_vs);
	m_bloom_x_post.attachShader(bloom_x_post_fs);
	m_bloom_x_post.link();

	// the blurring in y direction
	Candy::GL::GLShader bloom_y_post_fs("../etc/shaders/postprocess_bloom_y_blur.fs", Candy::GL::GLShaderType::FRAGMENT_SHADER);
	m_bloom_y_post.initialize();
	m_bloom_y_post.attachShader(passthrough_vs);
	m_bloom_y_post.attachShader(bloom_y_post_fs);
	m_bloom_y_post.link();
}

void Candy::DeferredPipeline::ItlInitializeSSAO()
{
	Candy::GL::GLShader ssao_post_vs("../etc/shaders/postprocess_passtrough.vs", Candy::GL::GLShaderType::VERTEX_SHADER);
	Candy::GL::GLShader ssao_post_fs("../etc/shaders/postprocess_ssao.fs", Candy::GL::GLShaderType::FRAGMENT_SHADER);
	m_ssao_post.initialize();
	m_ssao_post.attachShader(ssao_post_vs);
	m_ssao_post.attachShader(ssao_post_fs);
	m_ssao_post.link();
}

void Candy::DeferredPipeline::ItlInitializeLightningPass()
{
	// generate post processing buffers
	glGenFramebuffers(POST_FBO_NUM , m_postFbo);
	glGenTextures(POST_TEXTURE_NUM, m_color);

	for (unsigned int i = 0; i < POST_FBO_NUM; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_postFbo[i]);
		glBindTexture(GL_TEXTURE_2D, m_color[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_color[i], 0);

		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (Status != GL_FRAMEBUFFER_COMPLETE) {
			printf("FB error, status: 0x%x\n", Status);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(POST_FBO_NUM, m_bloom_postFbo);
	glGenTextures(POST_TEXTURE_NUM, m_bloom_color);

	for (unsigned int i = 0; i < POST_FBO_NUM; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_bloom_postFbo[i]);
		glBindTexture(GL_TEXTURE_2D, m_bloom_color[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, GLsizei(width / BLOOM_CROP), GLsizei(height / BLOOM_CROP), 0, GL_RGB, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_bloom_color[i], 0);

		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (Status != GL_FRAMEBUFFER_COMPLETE) {
			printf("FB error, status: 0x%x\n", Status);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glGenTextures(1, &m_screenshot);
	glBindTexture(GL_TEXTURE_2D, m_screenshot);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
}

void Candy::DeferredPipeline::beginShadowPass(Candy::Scene::Pointlight& light, unsigned direction, const glm::mat4& test, const glm::mat4& test2)
{
  m_shadowmapShader->bind();
 
  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  glCullFace(GL_FRONT);

  glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, light.m_fbo[0]);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + direction, light.m_shadowMap, 0);
  glDrawBuffer(GL_COLOR_ATTACHMENT0);

  glViewport(0, 0, light.fboSize, light.fboSize);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_shadowmapShader->setUniform("gMVP", light.getMVP((Candy::Scene::PointlightDirections)direction));
  m_shadowmapShader->setUniform("lightpos", light.position);
}

void Candy::DeferredPipeline::endShadowPass(RenderDevice* device, const Candy::Scene::Pointlight& light, unsigned direction)
{
  m_bloom_x_post.bind();

  // disable depth test
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  // copy one cubemap face to a seperate texture
  glBindFramebuffer(GL_FRAMEBUFFER, light.m_fbo[0]);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + direction, light.m_shadowMap, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, light.m_tmpTex);
  glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, light.fboSize, light.fboSize);

  // blur the copied texture, write back to the framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, light.m_fbo[0]);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + direction, light.m_shadowMap, 0);
  m_bloom_x_post.setUniform("color_tex", 0);
  m_bloom_x_post.setUniform("bloomCrop", 1.0f);
  m_bloom_x_post.setUniform("framebuffer_dim", glm::vec2(light.fboSize));
  device->draw(fboQuadGeo, fboVertexFormat);

  // copy one cubemap face to a seperate texture
  glBindFramebuffer(GL_READ_FRAMEBUFFER, light.m_fbo[0]);
  glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + direction, light.m_shadowMap, 0);
  glReadBuffer(GL_COLOR_ATTACHMENT0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, light.m_tmpTex);
  glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, light.fboSize, light.fboSize);

  m_bloom_y_post.bind();
  glBindFramebuffer(GL_FRAMEBUFFER, light.m_fbo[0]);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + direction, light.m_shadowMap, 0);
  glActiveTexture(GL_TEXTURE0);
  m_bloom_y_post.setUniform("color_tex", 0);
  m_bloom_y_post.setUniform("bloomCrop", 1.0f);
  m_bloom_y_post.setUniform("framebuffer_dim", glm::vec2(light.fboSize));

  device->draw(fboQuadGeo, fboVertexFormat);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}