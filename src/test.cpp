
#include <iostream>
#include <glm/detail/type_vec3.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp> 
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

#include <FreeImage.h>

#include <irrKlang.h>

#include <CandyApplication.h>
#include <Scene/Pointlight.h>
#include <GL/GLShader.h>
#include <GL/GLProgram.h>
#include <PrimitiveGenerator.h>

#include <memory>
#include <VertexFormat.h>
#include <GeometryBuffer.h>
#include <Geometry.h>
#include <DeferredPipeline.h>
#include <MeshLoader.h>
#include "utils.h"
#include "INIReader.h"

#include <sstream>
#include <fstream>
#include <map>

#include <iostream>

void copy_mat(const aiMatrix4x4 *from, glm::mat4 &to)
{
  to[0][0] = from->a1; to[1][0] = from->a2;
  to[2][0] = from->a3; to[3][0] = from->a4;
  to[0][1] = from->b1; to[1][1] = from->b2;
  to[2][1] = from->b3; to[3][1] = from->b4;
  to[0][2] = from->c1; to[1][2] = from->c2;
  to[2][2] = from->c3; to[3][2] = from->c4;
  to[0][3] = from->d1; to[1][3] = from->d2;
  to[2][3] = from->d3; to[3][3] = from->d4;
}

glm::vec3 bezier(const std::vector<glm::vec3>& points, int startIdx, float t)
{
  return (1.f - t) * (1.f - t) * (1.f - t) * points[startIdx] + 3.f * t * (1.f - t) * (1.f - t) * points[startIdx + 1] + 3.f * t * t * (1.f - t) * points[startIdx + 2] + t * t * t * points[startIdx + 3];
}

uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
  for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
    if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
      return i;
    }
  }

  assert(0);

  return 0;
}


uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
  assert(pNodeAnim->mNumRotationKeys > 0);

  for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
    if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
      return i;
    }
  }

  assert(0);

  return 0;
}


uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
  assert(pNodeAnim->mNumScalingKeys > 0);

  for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
    if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
      return i;
    }
  }

  assert(0);

  return 0;
}

void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
  if (pNodeAnim->mNumPositionKeys == 1) {
    Out = pNodeAnim->mPositionKeys[0].mValue;
    return;
  }

  uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
  uint NextPositionIndex = (PositionIndex + 1);
  assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
  float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
  float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
  assert(Factor >= 0.0f && Factor <= 1.0f);
  const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
  const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
  aiVector3D Delta = End - Start;
  Out = Start + Factor * Delta;
}

void CalcInterpolatedRotation(glm::mat4& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
  // we need at least two values to interpolate...
  if (pNodeAnim->mNumRotationKeys == 1) 
  {
    const auto& q = pNodeAnim->mRotationKeys[0].mValue;
    glm::quat gq(q.x, q.y, q.z, q.w);
    Out = glm::mat4_cast(gq);
    return;
  }

  uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
  uint NextRotationIndex = (RotationIndex + 1);
  assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
  float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
  float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
  assert(Factor >= 0.0f && Factor <= 1.0f);
  
  const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
  const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
  const glm::quat start(StartRotationQ.w, StartRotationQ.x, StartRotationQ.y, StartRotationQ.z);
  const glm::quat end(EndRotationQ.w, EndRotationQ.x, EndRotationQ.y, EndRotationQ.z);

  const glm::vec3 dirStart = glm::eulerAngles(start);
  const glm::vec3 dirEnd = glm::eulerAngles(end);

  glm::quat rot = glm::shortMix(start, end, Factor);
  //aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
  Out = glm::mat4_cast(rot);
}


void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
  if (pNodeAnim->mNumScalingKeys == 1) {
    Out = pNodeAnim->mScalingKeys[0].mValue;
    return;
  }

  uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
  uint NextScalingIndex = (ScalingIndex + 1);
  assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
  float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
  float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
  assert(Factor >= 0.0f && Factor <= 1.0f);
  const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
  const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
  aiVector3D Delta = End - Start;
  Out = Start + Factor * Delta;
}

const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName)
{
  for (uint i = 0; i < pAnimation->mNumChannels; i++) {
    const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

    if (string(pNodeAnim->mNodeName.data) == NodeName) {
      return pNodeAnim;
    }
  }

  return NULL;
}

glm::mat4 globalInverse;

void ReadNodeHeirarchy(aiAnimation* pAnimation, float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform, const std::vector<std::string>& bonenames, const std::vector<glm::mat4>& inversePoses, std::vector<glm::mat4>& finalTransformation)
{
  string NodeName(pNode->mName.data);

  glm::mat4 NodeTransformation(1.f);
  copy_mat(&pNode->mTransformation, NodeTransformation);

  const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

  if (pNodeAnim) 
  {
    // Interpolate scaling and generate scaling transformation matrix
    aiVector3D Scaling;
    CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
    glm::mat4 ScalingM = glm::scale(glm::mat4(1.0f), glm::vec3(Scaling.x, Scaling.y, Scaling.z));

    // Interpolate rotation and generate rotation transformation matrix
    glm::mat4 RotationM;
    CalcInterpolatedRotation(RotationM, AnimationTime, pNodeAnim);
    
    // Interpolate translation and generate translation transformation matrix
    aiVector3D Translation;
    CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
    glm::mat4 TranslationM = glm::translate(glm::mat4(1.f), glm::vec3(Translation.x, Translation.y, Translation.z));
    
    // Combine the above transformations
    NodeTransformation = TranslationM * RotationM * ScalingM;
  }

  int BoneIndex = find(bonenames.begin(), bonenames.end(), NodeName) - bonenames.begin();

  //REMOVE ME
//  if (BoneIndex < 40)
  //NodeTransformation = glm::mat4(1);// glm::inverse(inversePoses[BoneIndex]);

  glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;


  if (BoneIndex < 40) // TODO: remove magic number!!!!!!!!!!
  {
    //uint BoneIndex = m_BoneMapping[NodeName];
    finalTransformation[BoneIndex] = globalInverse * GlobalTransformation * inversePoses[BoneIndex];
  }

  for (uint i = 0; i < pNode->mNumChildren; i++) 
  {
    ReadNodeHeirarchy(pAnimation, AnimationTime, pNode->mChildren[i], GlobalTransformation, bonenames, inversePoses, finalTransformation);
  }
}

void BoneTransform(aiAnimation* pAnimation, aiNode* root, float TimeInSeconds, const std::vector<std::string>& bonenames, const std::vector<glm::mat4>& inversePoses, std::vector<glm::mat4>& finalTransformation)
{
  /*Matrix4f Identity;
  Identity.InitIdentity();*/

  float TicksPerSecond = pAnimation->mTicksPerSecond != 0 ? pAnimation->mTicksPerSecond : 25.0f;
  float TimeInTicks = TimeInSeconds * TicksPerSecond / 2.0f;
  float AnimationTime = fmod(TimeInTicks, pAnimation->mDuration);

  ReadNodeHeirarchy(pAnimation, AnimationTime, root, glm::mat4(1.f), bonenames, inversePoses, finalTransformation);

/*  Transforms.resize(pAnimation->mNu);

  for (uint i = 0; i < m_NumBones; i++) {
    Transforms[i] = m_BoneInfo[i].FinalTransformation;
  }*/
}

void _check_gl_error(const char *file, int line);
#define check_gl_error() _check_gl_error(__FILE__,__LINE__)
void _check_gl_error(const char *file, int line) 
{
  GLenum err(glGetError());

  while (err != GL_NO_ERROR) {
    string error;

    switch (err) {
    case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
    case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
    case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
    case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
    }

    cerr << "GL_" << error.c_str() << " - " << file << ":" << line << endl;
    err = glGetError();
  }
}

struct Vertex
{
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texcoord;
};

struct Vertex2
{
  glm::vec3 position;
  glm::vec3 color;
};


class TestApp : public Candy::CandyApplication
{
public:
  TestApp()
  {
	  pSceneShape = nullptr;

    // load the file into the scene
    const aiScene* pLoadedScene = importer.ReadFile("../etc/gangnamstyle/hellknight_draft.dae", aiProcess_Triangulate | aiProcess_GenSmoothNormals);
    glm::mat4 globalTransform;
    const auto tmp = pLoadedScene->mRootNode->mTransformation;
    copy_mat(&tmp, globalTransform);
    globalInverse = glm::inverse(globalTransform);

    if (!pLoadedScene)
    {
      std::cout << "model loading failed" << std::endl;
    }

    const auto* _mesh = pLoadedScene->mMeshes[0];
    anim = pLoadedScene->mAnimations[0];
    rootnode = pLoadedScene->mRootNode;

    if (_mesh->HasBones())
    {
      for (int b = 0; b < _mesh->mNumBones; b++)
      {
        aiBone* bone = _mesh->mBones[b];
        std::cout << bone->mName.C_Str() << std::endl;
        const aiMatrix4x4 aiInvBindPose = bone->mOffsetMatrix;
        glm::mat4 invBindPose(1.f);
        copy_mat(&aiInvBindPose, invBindPose);
        bones.push_back(invBindPose);
        bonenames.push_back(bone->mName.C_Str());
      }
    }

	//init sound
	soundEngine = irrklang::createIrrKlangDevice();
  }
  ~TestApp()
  {
//	  SAFE_DELETE(pbuffer);
	  // SAFE_DELETE(pSceneShape); ERROR Sadly this pointer got lost somewhere...
    if (gangnamstyle)
      gangnamstyle->drop(); // release music stream.
    if (dungeon)
      dungeon->drop(); // release music stream.

    soundEngine->drop(); // delete engine
  }

  void loadShaders()
  {
	  Candy::GL::GLShader deferred_passtrough_vs("../etc/shaders/deferred_passtrough.vs", Candy::GL::GLShaderType::VERTEX_SHADER);
	  Candy::GL::GLShader deferred_passtrough_fs("../etc/shaders/deferred_passtrough.fs", Candy::GL::GLShaderType::FRAGMENT_SHADER);
	  deferred_passthrough_shader.initialize();
	  deferred_passthrough_shader.attachShader(deferred_passtrough_vs);
	  deferred_passthrough_shader.attachShader(deferred_passtrough_fs);
	  deferred_passthrough_shader.link();
    Candy::GL::GLShader phong_tess_vs("../etc/shaders/phong_tess.vs", Candy::GL::GLShaderType::VERTEX_SHADER);
    //Candy::GL::GLShader phong_tess_tctrl("../etc/shaders/phong_tess.tctrl", Candy::GL::GLShaderType::TESS_CONTROL_SHADER);
    //Candy::GL::GLShader phong_tess_teval("../etc/shaders/phong_tess.teval", Candy::GL::GLShaderType::TESS_EVALUATION_SHADER);
    Candy::GL::GLShader phong_tess_fs("../etc/shaders/phong_tess.fs", Candy::GL::GLShaderType::FRAGMENT_SHADER);
    phong_tess.initialize();
    phong_tess.attachShader(phong_tess_vs);
    //phong_tess.attachShader(phong_tess_tctrl);
    //phong_tess.attachShader(phong_tess_teval);
    phong_tess.attachShader(phong_tess_fs);
    phong_tess.link();
    phong_tess.setPrimitiveMode(Candy::PrimitiveMode::PATCHES);
  }

  void initialize() override
  {
	  INIReader reader("../etc/config.ini");
	  if (reader.ParseError() < 0) {
		  std::cout << "Can't load '../etc/config.ini'\n";
		  system("PAUSE");
		  exit(EXIT_FAILURE);
	  }

	  //screen
	  bool fullscreen = reader.GetBoolean("start", "fullscreen", true);
	  width = reader.GetInteger("start", "width", 1024);
	  height = reader.GetInteger("start", "height", 768);
	  shadowMapSize = reader.GetInteger("lights", "shadowmapsize", 256);

	  getContext()->createRenderingContext(width, height, fullscreen, "Test1", 4, 4);
	  loadShaders();

	  pipeline.initialize(width, height);

    sphereMarker = new Candy::GeometryBuffer();
    Candy::IndexedMesh<Vertex2, unsigned int> tmp_mesh;
    Candy::PrimitiveGeometry::sphere(tmp_mesh, 10.f, 1);
	simpleVFormat = new Candy::VertexFormat();
	simpleVFormat->addVertexElement(0, Candy::FLOAT_VEC3, 0, Candy::POSITION);
	simpleVFormat->addVertexElement(1, Candy::FLOAT_VEC3, 12, Candy::COLOR);
    sphereMarker->upload(tmp_mesh.vertices, tmp_mesh.indices);

	  meshLoader = Candy::MeshLoader();

    pHellknight = meshLoader.LoadMesh("../etc/gangnamstyle/hellknight_draft.dae");
    pHellknight->SetRenderingTechnique(pipeline.GetPassthroughShader());

    pHellknight->SetCulling(false);

	  pSceneShape = meshLoader.LoadMesh("../etc/crytek_sponza/sponza.obj");
    pSceneShape->SetRenderingTechnique(pipeline.GetPassthroughShader());

	InitCamera();
	InitLights();




//    pSceneShape->SetRenderingTechnique(&phong_tess);
    check_gl_error();

    dungeon = soundEngine->play2D("../etc/audio/dungeon.ogg", false, false, true);
    gangnamstyle = soundEngine->play2D("../etc/audio/gangnamstyle.ogg", true, true, true);
  }

  void InitLights();

  void InitCamera();

  void update() override
  {
	  Candy::RenderDevice* pRenderDevice = getContext();
      //std::cout << TestApp::getFramesPerSecond() << " FPS " << TestApp::getFrameNumber() << std::endl
	  if (pRenderDevice->keydown(GLFW_KEY_F5))
	  {
		  // ERROR: Loads the shaders multible time because this event gets cought multible times
		  std::cout << "Reload Shaders" << std::endl;
		  loadShaders();
	  }

    //pipeline.ItlInitializeBloom();
    //loadShaders();
  }

  void render() override
  {
    glLineWidth(5);

    if (getContext()->IsCameraFreeMoveable())
    {
      pCamera->updateKeyboard(getFrameTime());
    }
    else if (pCamera->hasPath())
    {
      pCamera->updatePath(getAbosulteTime() / 2.f);

      /*auto worldMat = glm::inverse(pCamera->getWorldMatrix());
      auto worldCamPos = worldMat * glm::vec4(0, 0, 0, 1);
      auto worldCamDir = worldMat * glm::vec4(0, 0, 1, 0);
      std::cout << "pos [ " << worldCamPos[0] << ", " << worldCamPos[1] << ", " << worldCamPos[2] << " ]" << std::endl;
      std::cout << "dir [ " << worldCamDir[0] << ", " << worldCamDir[1] << ", " << worldCamDir[2] << " ]" << std::endl;*/
    }

    bool isPaused = gangnamstyle->getIsPaused();
    if (getAbosulteTime() > 46.0 && isPaused)
    {
      gangnamstyle->setIsPaused(false);
    }

    //glm::vec3 pos = bezier(path, 0, fmod<float>(getAbosulteTime() / 8.0f, 1.f));
    //glm::mat4 pos = hermiteSpline(path_points, path_directions, static_cast<float>(getAbosulteTime()) / 8.0f);

    //auto rotate_mat = glm::rotate(glm::mat4(1.f), fmod<float>(getAbosulteTime() / 8.0f, 360.f), glm::vec3(0, 1, 0));
    //rotate_mat = glm::translate(glm::mat4(1), pos);

    pipeline.beginGeometryPass();
    check_gl_error();
    //    model1 = pos;
    //pCamera->setPosition(glm::vec3(pos[3][0], pos[3][1], pos[3][2]));

    // draw hellknight
    auto transform = glm::mat4(1);
    transform = glm::scale(transform, glm::vec3(25));
    transform = glm::rotate(transform, -3.1415f / 2.f, glm::vec3(1, 0, 0));
    transform = glm::rotate(transform, -3.1415f / 2.f, glm::vec3(0, 0, 1));
    transform = glm::translate(transform, glm::vec3(-1, 15, 0));

    std::vector<glm::mat4> finalTransform(bones.size());
    BoneTransform(anim, rootnode, getAbosulteTime() * 2.f, bonenames, bones, finalTransform);

    auto lightRot = [](float r, float t, float o, float h)
    {
      return glm::vec3(-400.f+r*sin(t*(2.f*glm::pi<float>()+o)), h, r*cos(t*(2.f*glm::pi<float>()+o)));
    };
    
    for (auto light : pointlights)
    {
      light->updatePath(getAbosulteTime() / 2.0f);
      //pipeline.setMatrices(glm::translate(glm::mat4(1), light->position), pCamera->getWorldMatrix(), pCamera->getProjectionMatrix());
      //getContext()->draw(sphereMarker, simpleVFormat);
    }

    if (getAbosulteTime() > 51.0f)
    {
      float t = getAbosulteTime() / 8.f;
      pointlights[0]->position = lightRot(150.f, t, 2.f*glm::pi<float>() * 0, 450.f);
      pointlights[1]->position = lightRot(150.f, t, 2.f*glm::pi<float>() * 1.f / 3.f, 350.f);
      pointlights[2]->position = lightRot(150.f, t, 2.f*glm::pi<float>() * 2.f / 3.f, 300.f);

      pointlights[0]->color = glm::vec3(1.0, 0.3, 0.3) / 3.f;
      pointlights[1]->color = glm::vec3(0.3, 1.0, 0.3) / 3.f;
      pointlights[2]->color = glm::vec3(0.3, 0.3, 1.0) / 3.f;
    }


    float end = 112.0f;
    if (getAbosulteTime() > end && getAbosulteTime() < end+5.0f)
    {
      float darkness = (5.0f - (getAbosulteTime() - end))/5.0f;
      pointlights[0]->color = glm::vec3(1.0, 0.6, 0.6) / 6.f * darkness;
      pointlights[1]->color = glm::vec3(0.6, 1.0, 0.6) / 6.f * darkness;
      pointlights[2]->color = glm::vec3(0.6, 0.6, 1.0) / 6.f * darkness;
      gangnamstyle->setVolume(darkness);
    }

    if (getAbosulteTime() > end + 5.f)
      exit(0);

    check_gl_error();

    pipeline.setMatrices(glm::mat4(1), pCamera->getWorldMatrix(), pCamera->getProjectionMatrix());
    if (getAbosulteTime() > 47.0f)
    {
     
      pipeline.GetPassthroughShaderAnimated()->bind();
      pHellknight->SetRenderingTechnique(pipeline.GetPassthroughShaderAnimated());
      pipeline.GetPassthroughShaderAnimated()->setUniform("gBoneMatrix", finalTransform);
      pipeline.GetPassthroughShaderAnimated()->setUniform("gWorld", transform);
      pipeline.GetPassthroughShaderAnimated()->setUniform("gNormal", glm::mat3(glm::transpose(glm::inverse(pCamera->getWorldMatrix()))));
      pipeline.GetPassthroughShaderAnimated()->setUniform("gWVP", pCamera->getProjectionMatrix() * pCamera->getWorldMatrix() * transform);
      pHellknight->draw(getContext());
    }

    // draw sponza
    pipeline.GetPassthroughShader()->bind();

    if (pCamera->inFrustum(*pSceneShape))
      pSceneShape->draw(getContext());

    // draw camera trail
    /*pCamera->drawTrail(getContext());
    for (auto light : pointlights)
    {
      if(light->hasPath())
        light->drawTrail(getContext());
    }*/

    //pipeline.setMatrices(glm::translate(glm::mat4(1), glm::vec3(50, 0, -200)), pCamera->getWorldMatrix(), pCamera->getProjectionMatrix());
    //getContext()->draw(sphereMarker, simpleVFormat);
    check_gl_error();
    // std::cout << pCamera->getNumberOfShownObjects() << std::endl;
    pipeline.endGeometryPass();

    for (Candy::Scene::Pointlight* light : pointlights) // l = 0; l < pointlights.size(); l++)
    {
      /*pipeline.setMatrices/, pCamera->getWorldMatrix(), pCamera->getProjectionMatrix());*/
      int vis = 0;
      for (unsigned dir = 0; dir < 6; dir++)
      {
        pipeline.beginShadowPass(*light, dir, pCamera->getWorldMatrix(), pCamera->getProjectionMatrix());
        pipeline.GetShadowpassShader()->setUniform("modelMatrix", glm::mat4(1));
        light->update((Candy::Scene::PointlightDirections)dir);
        check_gl_error();

        for (unsigned int n = 0; n < pSceneShape->m_vpMeshParts.size(); n++)
        {
          if (light->inFrustum(*pSceneShape->m_vpMeshParts[n], dir))
          {
            getContext()->draw(pSceneShape->m_vpMeshParts[n], pSceneShape->m_pVertexFormat, Candy::PrimitiveMode::TRIANGLES);
            vis++;
          }
        }

        /*pipeline.GetShadowpassShader()->setUniform("modelMatrix", glm::translate(glm::mat4(1), glm::vec3(50, 0, 200)));
        //pipeline.setMatrices(glm::translate(glm::mat4(1), glm::vec3(100,0,100)), pCamera->getWorldMatrix(), pCamera->getProjectionMatrix());
        getContext()->draw(sphereMarker, simpleVFormat);*/

        if (getAbosulteTime() > 47.0f)
        {
          pipeline.GetShadowpassShaderAnimated()->bind();
          pipeline.GetShadowpassShaderAnimated()->setUniform("gBoneMatrix", finalTransform);
          pipeline.GetShadowpassShaderAnimated()->setUniform("gWorld", transform);
          pipeline.GetShadowpassShaderAnimated()->setUniform("gNormal", glm::mat3(glm::transpose(glm::inverse(transform))));
          pipeline.GetShadowpassShaderAnimated()->setUniform("gWVP", light->getMVP((Candy::Scene::PointlightDirections)dir) * transform);
          pipeline.GetShadowpassShaderAnimated()->setUniform("lightpos", light->position);
          for (unsigned int n = 0; n < pHellknight->m_vpMeshParts.size(); n++)
          {
            getContext()->draw(pHellknight->m_vpMeshParts[n], pHellknight->m_pVertexFormat, Candy::PrimitiveMode::TRIANGLES);
          }
        }

        pipeline.endShadowPass(getContext(), *light, dir);

        check_gl_error();
      }

      //std::cout << vis << " " << pSceneShape->m_vpMeshParts.size() * 6 << std::endl;

      /*glBindTexture(GL_TEXTURE_CUBE_MAP, light->m_shadowMapBlurred);
      glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
      glBindTexture(GL_TEXTURE_CUBE_MAP, 0);*/
    }
    //return;

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    check_gl_error();
    //pipeline.drawCubeMap(pCamera->getViewMatrix(), pCamera->getProjectionMatrix());

    pipeline.beginLightingPass();
    check_gl_error();

    deferred_passthrough_shader.bind();

    pipeline.bindTextures();
    check_gl_error();

    auto camPos = pCamera->getPosition();
    //std::cout << tmp.x << " " << tmp.y << " " << tmp.z << " " << std::endl;

    for (auto* light : pointlights)
    {
      deferred_passthrough_shader.setUniform("cameraPos", pCamera->getPosition());
      deferred_passthrough_shader.setUniform("lightpos", light->position);
      deferred_passthrough_shader.setUniform("lightpos", light->position);
      deferred_passthrough_shader.setUniform("lightcolor", light->getColor());
      deferred_passthrough_shader.setUniform("framebuffer_dim", glm::vec2(width, height));
      check_gl_error();

      int loc = deferred_passthrough_shader.getUniformByName("shadowTex");
      //assert(loc > -1);
      glActiveTexture(GL_TEXTURE0 + 12);
      glBindTexture(GL_TEXTURE_CUBE_MAP, light->m_shadowMap);
      glUniform1i(loc, 12);

      pipeline.endLightingPass(getContext());
      check_gl_error();
    }

    glDisable(GL_BLEND);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    pipeline.runPostprocess(*getContext());
    check_gl_error();

    pipeline.displayColorbuffer();
    check_gl_error();

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    check_gl_error();

    if (getContext()->keydown(GLFW_KEY_BACKSPACE))
    {
      pCamera->clearPath();
    }

    /*if (glm::length(pCamera->getLastPathPosition() - pCamera->getPosition()) > 100.0f && getContext()->IsCameraFreeMoveable())
    {
    pCamera->addNodeToPath(pCamera->getPosition(), pCamera->getDirection());
    std::cout << glm::to_string(pCamera->getPosition()) << " " << glm::to_string(pCamera->getDirection()) << std::endl;
    pCamera->genTrail();
    }*/

    //std::cout << "pCamera->addNodeToPath({ " << glm::to_string(pCamera->getPosition()) << " }, { " << glm::to_string(pCamera->getDirection()) << "}" << std::endl;

    if (getContext()->keydown(GLFW_KEY_SPACE))
    { 
      auto p = pCamera->getPosition();
      auto d = pCamera->getDirection();

      auto toStr = [](glm::vec3 v) -> std::string { return std::to_string(v[0]) + std::string(", ") + std::to_string(v[1]) + std::string(", ") + std::to_string(v[2]); };

      getContext()->setClipboardString("pathObj->addNodeToPath({ " + toStr(p) + " }, { " + toStr(d) + "});");
    }

    if (getFrameNumber() % 20 == 0)
      std::cout << getFramesPerSecond() << std::endl;
  }

private:
  Candy::Scene::Camera*		pCamera;
  Candy::Scene::SceneShape*	pSceneShape;
  Candy::Scene::SceneShape*	pHellknight;
  Candy::MeshLoader			meshLoader; 
  Candy::GL::GLProgram deferred_passthrough_shader;
  Candy::GL::GLProgram phong_tess;
  glm::mat4 model1,view,projection;

  Candy::DeferredPipeline pipeline;

  Candy::VertexFormat* simpleVFormat;
  Candy::GeometryBuffer* sphereMarker, cubeMarker;

  std::vector<Candy::Scene::Pointlight*> pointlights;

  Assimp::Importer importer;
  std::vector<glm::mat4> bones;
  std::vector<std::string> bonenames;
  aiAnimation* anim;
  aiNode* rootnode;

  int width,
	    height,
	    shadowMapSize;

  irrklang::ISoundEngine* soundEngine;
  irrklang::ISound* gangnamstyle;
  irrklang::ISound* dungeon;
};


glm::vec3 dir(glm::vec3 pos, glm::vec3 dest)
{
  return glm::normalize(dest - pos);
}

void TestApp::InitCamera()
{
	/*camera_path.positions.push_back(glm::vec3(-661.78, -10.26, -882.08));
	  camera_path.positions.push_back(glm::vec3(362.92, -39.25, -245.24));

	  camera_path.directions.push_back(glm::vec3(-0.6731, -0.0591, 0.737176));
	  camera_path.directions.push_back(glm::vec3(-0.60772, 0.023821, -0.793794));*/

	pCamera = getContext()->getCamera();
  Candy::Scene::SceneObject* pathObj = pCamera;
  pathObj->addNodeToPath({ -945.151672, 119.046722, -542.040039 }, dir({ -945.151672, 119.046722, -542.040039 }, { -1377.331543, 177.202209, -35.756428 }));
  pathObj->addNodeToPath({ -1087.976563, 208.261566, -148.396561 }, dir({ -1087.976563, 208.261566, -148.396561 }, { -1377.331543, 177.202209, -35.756428 }));
  pathObj->addNodeToPath({ -1150.630371, 208.261490, 262.611908 }, dir({ -1150.630371, 208.261490, 262.611908 }, { -1377.331543, 177.202209, -35.756428 }));
  pathObj->addNodeToPath({ -1191.993042, 143.501068, 137.852020 }, { -0.575351, -0.139543, 0.805915 });
  pathObj->addNodeToPath({ -1228.171753, 83.722755, 87.055641 }, { 0.522582, -0.019999, 0.852354 });
  pathObj->addNodeToPath({ -1035.468628, 95.502251, 383.185516 }, { 0.998103, 0.010000, 0.060756 });
  pathObj->addNodeToPath({ -596.285461, 64.641853, 503.147522 }, { 0.761153, 0.009204, -0.648507 });
  pathObj->addNodeToPath({ 200.390945, 51.795479, 528.081360 }, { 0.573140, -0.059964, -0.817261 });
  pathObj->addNodeToPath({ 832.839355, 68.086700, 423.071106 }, { 0.732735, -0.010796, -0.680428 });
  pathObj->addNodeToPath({ 1087.635498, 104.008644, 241.365524 }, { -0.960772, -0.069943, -0.268376 });
  pathObj->addNodeToPath({ 1116.968262, 171.147812, 117.429810 }, { -0.953790, -0.199450, 0.224733 });
  pathObj->addNodeToPath({ 936.874756, 210.493530, -130.802124 }, { -0.444182, 0.265964, 0.855550 });
  pathObj->addNodeToPath({ 803.566589, 343.961609, -98.102539 }, { -0.400069, 0.716801, 0.571088 });
  pathObj->addNodeToPath({ 632.835327, 596.412781, 150.858566 }, { -0.435161, 0.379452, 0.816487 });
  pathObj->addNodeToPath({ 588.913391, 550.099426, 368.941101 }, { -0.997227, -0.050775, 0.054413 });
  pathObj->addNodeToPath({ 273.171204, 623.921021, 350.204102 }, { -0.968362, -0.040785, -0.246196 });
  pathObj->addNodeToPath({ -207.132095, 665.519470, 515.736328 }, { -0.431906, -0.169967, -0.885759 });
  pathObj->addNodeToPath({ -873.289124, 729.835571, 426.682434 }, { 0.683134, -0.315322, -0.658711 });
  pathObj->addNodeToPath({ -873.289246, 729.835571, 426.682495 }, { 0.011228, -0.557023, -0.830422 });
  pathObj->addNodeToPath({ -1366.339844, 729.835632, 216.961334 }, { 0.866227, -0.238476, -0.439068 });//21
  pathObj->addNodeToPath({ -1294.044434, 738.686035, 94.370255 }, { 0.932722, -0.352274, -0.077021 });
  pathObj->addNodeToPath({ -1294.044434, 738.686035, 94.370255 }, { 0.932722, -0.352274, -0.077021 }); // still
  pathObj->addNodeToPath({ -938.439453, 575.860168, 49.303135 }, { 0.976904, -0.188077, -0.101418 });
  pathObj->addNodeToPath({ -588.350891, 666.257507, 23.660593 }, { 0.939684, 0.332736, -0.079248 });
  pathObj->addNodeToPath({ 212.489761, 892.268066, -31.471794 }, { 0.953315, 0.294759, -0.065629 });
  pathObj->addNodeToPath({ 483.421478, 989.995239, -47.653809 }, { 0.901380, 0.246632, -0.355931 });
  pathObj->addNodeToPath({ 483.421448, 989.995178, -47.653812 }, { 0.140904, -0.238476, -0.960872 });
  pathObj->addNodeToPath({ 483.421387, 989.995178, -47.653820 }, { -0.732270, -0.581683, -0.354154 });
  pathObj->addNodeToPath({ 88.571526, 766.754700, -253.871948 }, { -0.555626, -0.621610, 0.552160 });
  pathObj->addNodeToPath({ -223.466446, 448.207092, -218.697968 }, { 0.449150, -0.435682, 0.780029 });
  pathObj->addNodeToPath({ -322.846832, 288.300659, -44.302685 }, { 0.899691, -0.305817, 0.311499 });
  pathObj->addNodeToPath({ -162.155807, 228.102707, 13.693399 }, { 0.979387, -0.199450, -0.031956 });
  pathObj->addNodeToPath({ -16.226488, 152.282806, -26.374886 }, { 0.995952, -0.089879, 0.000793 });
  pathObj->addNodeToPath({ -180.656036, 151.014771, -48.965164 }, { 0.999897, -0.010796, -0.009450 });
  pathObj->addNodeToPath({ -474.608398, 174.823944, -42.809658 }, { 0.996737, -0.080709, 0.001431 });
  pathObj->addNodeToPath({ -952.821594, 204.865265, -29.968884 }, { 0.997441, -0.070737, -0.010395 });
  pathObj->addNodeToPath({ -1258.463501, 45.745396, -33.759533 }, { 0.969065, 0.246632, 0.009199 });
  pathObj->addNodeToPath({ -934.557007, 162.516891, -55.053947 }, { 0.799781, 0.538961, -0.264332 });
  pathObj->addNodeToPath({ -810.129150, 323.489227, -112.810326 }, { 0.353525, 0.504846, -0.787497 });
  pathObj->addNodeToPath({ -573.306702, 294.360840, 138.854614 }, { 0.122955, 0.207681, -0.970439 });
  pathObj->addNodeToPath({ -46.133522, 292.988251, 177.328278 }, { 0.088538, 0.178246, -0.979995 });
  pathObj->addNodeToPath({ 299.729736, 361.917816, 119.587387 }, { 0.692273, 0.197889, -0.693973 });
  pathObj->addNodeToPath({ 630.510620, 512.570679, -6.786072 }, { 0.861502, 0.370181, -0.347534 });
  pathObj->addNodeToPath({ 1160.448242, 570.730530, -349.852081 }, { -0.969473, 0.079121, -0.232083 });
  pathObj->addNodeToPath({ 989.534973, 574.330688, -409.735535 }, { -0.996183, 0.029199, -0.082261 });
  pathObj->addNodeToPath({ 521.263306, 674.369507, -543.982544 }, { -0.727484, -0.207681, 0.653939 });
  pathObj->addNodeToPath({ 216.748062, 675.825195, -527.459717 }, { -0.553984, -0.265964, 0.788901 });
  pathObj->addNodeToPath({ -125.801575, 657.809021, -552.015320 }, { -0.488999, -0.285189, 0.824346 });
  pathObj->addNodeToPath({ -488.225494, 656.463623, -539.868103 }, { -0.350744, -0.228753, 0.908103 });
  pathObj->addNodeToPath({ -785.318420, 668.927795, -416.669861 }, { 0.794683, -0.286715, 0.535045 });
  pathObj->addNodeToPath({ -779.972290, 667.302246, -243.815704 }, { 0.881758, -0.453596, 0.129438 });
  pathObj->addNodeToPath({ -787.351013, 667.302246, -169.480164 }, { 0.863181, -0.497571, 0.085682 });
  pathObj->addNodeToPath({ -789.932983, 667.302246, 10.470567 }, { 0.890861, -0.453596, -0.024839 });
  pathObj->addNodeToPath({ -692.249268, 615.718872, -119.040794 }, { 0.942157, -0.324796, 0.082755 });
  pathObj->addNodeToPath({ -612.173645, 807.540344, -26.577000 }, { 0.731143, -0.682221, -0.002130 });
  pathObj->addNodeToPath({ -717.700928, 1201.245605, -28.676792 }, { 0.688885, -0.724836, 0.007077 });
  pathObj->addNodeToPath({ -771.333191, 1512.853271, -26.724131 }, { 0.522441, -0.852525, 0.016052 });
  pathObj->addNodeToPath({ -819.073975, 1868.385498, -27.084240 }, { 0.443853, -0.896053, 0.009196 });
  pathObj->addNodeToPath({ -889.085938, 2230.133301, -29.294594 }, { 0.295275, -0.955337, 0.012029 });
  pathObj->addNodeToPath({ -953.805542, 2634.358887, -30.097397 }, { 0.266606, -0.963771, 0.008191 });
  pathObj->addNodeToPath({ -1042.105591, 3181.966553, -35.088478 }, { 0.314553, -0.949235, -0.002921 });

  //pCamera->addNodeToPath({ 100, 200, 100 }, look({ 100, 200, 100 }, { 0, 0, 0, }));


  /*pCamera->addNodeToPath({ 0, 100, 0 }, { 0, 0, 0.01 });
  pCamera->addNodeToPath({ 0, 300, 0 }, { 0, 0, 0.01 });*/
	/*pCamera->addNodeToPath({ 1019.03, 79.69, 438.92 }, { 0.038276, 0.07912, 0.99613 });
	pCamera->addNodeToPath({ 1040.09, 39.9, -441.71 }, { 0.996851, -0.0791206, -0.005292 });
	pCamera->addNodeToPath({ -1144.95, 71.24, -466.38 }, { -0.0045, -0.0192, -0.9998 });
	pCamera->addNodeToPath({ -1157.7, 67.895, 352.27 }, { -0.99888, -0.0391934, 0.0262631 });
	pCamera->addNodeToPath({ 1019.03, 79.69, 438.92 }, { 0.038276, 0.07912, 0.99613 });
	pCamera->addNodeToPath({ 1019.03, 79.69, 0 }, { 0.996851, -0.0791206, -0.005292 });
	pCamera->addNodeToPath({ 0, 79.69, 0 }, { 0.996851, -0.0791206, -0.005292 });*/

	pCamera->genTrail();

	/*camera_path.directions.push_back({ 0.038276, 0.07912, 0.99613 });
	camera_path.directions.push_back({ 0.038276, 0.07912, 0.99613 });
	camera_path.directions.push_back({ 0.038276, 0.07912, 0.99613 });*/
}

void addUpperCircle(Candy::Scene::SceneObject* pathObj)
{
  glm::vec3 A = { 1175.877441, 825.121826, -470.985687 };
  glm::vec3 B = { -949.722351, 674.074219, -470.256531 };
  glm::vec3 C = { -1395.012817, 815.514282, 383.418243 };
  glm::vec3 D = { 1055.309448, 724.912964, 445.786224 };

  pathObj->addNodeToPath({ A }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(A, B, 0.25f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(A, B, 0.5f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(A, B, 0.75f) }, { 0, 0, 0 });

  pathObj->addNodeToPath({ B }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(B, C, 0.25f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(B, C, 0.5f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(B, C, 0.75f) }, { 0, 0, 0 });

  pathObj->addNodeToPath({ C }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(C, D, 0.25f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(C, D, 0.5f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(C, D, 0.75f) }, { 0, 0, 0 });

  pathObj->addNodeToPath({ D }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(D, A, 0.25f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(D, A, 0.5f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(D, A, 0.75f) }, { 0, 0, 0 });
}

void TestApp::InitLights()
{
  glm::vec3 A = { 1175.877441, 825.121826, -470.985687 };
  glm::vec3 B = { -949.722351, 674.074219, -470.256531 };
  glm::vec3 C = { -1395.012817, 815.514282, 383.418243 };
  glm::vec3 D = { 1055.309448, 724.912964, 445.786224 };

	Candy::Scene::Pointlight* light0 = new Candy::Scene::Pointlight();
	light0->initialize(shadowMapSize);
	glm::vec3 direction(0);
	light0->color = glm::vec3(1, 0.6, 0.6) / 6.f; //using color instead of direction
  Candy::Scene::SceneObject* pathObj = light0;
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -300, 200, -185.722168 }, { -0.000431, -0.267499, 0.963558 });
  pathObj->addNodeToPath({ 300, 200, -185.722168 }, { -0.000431, -0.267499, 0.963558 });
  for (int i = 0; i < 8; i++)
    pathObj->addNodeToPath({ 700, 200, -185.722168 }, { -0.000431, -0.267499, 0.963558 });

  for (int i = 0; i < 18; i++)
    pathObj->addNodeToPath({ 0, 500, 0 }, { -0.000431, -0.267499, 0.963558 });

  /*pathObj->addNodeToPath({ 711.451172, 391.603760, -164.057159 }, { 0.461274, 0.886627, -0.033449 });
  pathObj->addNodeToPath({ 657.567749, 768.802124, -25.850376 }, { 0.543628, -0.343646, -0.765752 });
  pathObj->addNodeToPath({ 892.353760, 677.610046, -133.275604 }, { 0.906290, -0.179813, -0.382498 });
  pathObj->addNodeToPath({ 1090.203003, 650.334595, -279.802460 }, { -0.723645, -0.315322, -0.613929 });
  addUpperCircle(pathObj);

  pathObj->addNodeToPath({ A }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(A, B, 0.25f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(A, B, 0.5f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(A, B, 0.75f) }, { 0, 0, 0 });

  pathObj->addNodeToPath({ B }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(B, C, 0.25f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(B, C, 0.5f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(B, C, 0.75f) }, { 0, 0, 0 });

  pathObj->addNodeToPath({ C }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(C, D, 0.25f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(C, D, 0.5f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(C, D, 0.75f) }, { 0, 0, 0 });*/
  

	Candy::Scene::Pointlight* light1 = new Candy::Scene::Pointlight();
	light1->initialize(shadowMapSize);
	light1->color = glm::vec3(0.4, 0.8, 0.4) / 4.f;
  pathObj = light1;
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -300, 200, -185.722168 }, { -0.000431, -0.267499, 0.963558 });
  pathObj->addNodeToPath({ 300, 200, -185.722168 }, { -0.000431, -0.267499, 0.963558 });
  for (int i = 0; i < 8; i++)
    pathObj->addNodeToPath({ 700, 200, -185.722168 }, { -0.000431, -0.267499, 0.963558 });
  for (int i = 0; i < 18; i++)
    pathObj->addNodeToPath({ 0, 500, 0 }, { -0.000431, -0.267499, 0.963558 });

  /*pathObj->addNodeToPath({ D }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(D, A, 0.25f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(D, A, 0.5f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(D, A, 0.75f) }, { 0, 0, 0 });
  addUpperCircle(pathObj);
  pathObj->addNodeToPath({ A }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(A, B, 0.25f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(A, B, 0.5f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(A, B, 0.75f) }, { 0, 0, 0 });

  pathObj->addNodeToPath({ B }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(B, C, 0.25f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(B, C, 0.5f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(B, C, 0.75f) }, { 0, 0, 0 });*/

	Candy::Scene::Pointlight* light2 = new Candy::Scene::Pointlight();
	light2->initialize(shadowMapSize);
	light2->color = glm::vec3(0.4, 0.4, 1) / 4.f;
  pathObj = light2;
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -300, 200, -185.722168 }, { -0.000431, -0.267499, 0.963558 });
  pathObj->addNodeToPath({ 300, 200, -185.722168 }, { -0.000431, -0.267499, 0.963558 });
  for (int i = 0; i < 8; i++)
    pathObj->addNodeToPath({ 700, 200, -185.722168 }, { -0.000431, -0.267499, 0.963558 });
  for (int i = 0; i < 18; i++)
    pathObj->addNodeToPath({ 0, 500, 0 }, { -0.000431, -0.267499, 0.963558 });

  /*pathObj->addNodeToPath({ C }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(C, D, 0.25f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(C, D, 0.5f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(C, D, 0.75f) }, { 0, 0, 0 });

  pathObj->addNodeToPath({ D }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(D, A, 0.25f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(D, A, 0.5f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(D, A, 0.75f) }, { 0, 0, 0 });
  addUpperCircle(pathObj);
  pathObj->addNodeToPath({ A }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(A, B, 0.25f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(A, B, 0.5f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(A, B, 0.75f) }, { 0, 0, 0 });*/

	Candy::Scene::Pointlight* light3 = new Candy::Scene::Pointlight();
	light3->initialize(shadowMapSize);
	light3->color = glm::vec3(1.2, 0.4, 0.4) / 4.f;
  pathObj = light3;
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -600, 200, -170.512695 }, { 0.108999, -0.118921, 0.986903 });
  pathObj->addNodeToPath({ -300, 200, -185.722168 }, { -0.000431, -0.267499, 0.963558 });
  pathObj->addNodeToPath({ 300, 200, -185.722168 }, { -0.000431, -0.267499, 0.963558 });
  for (int i = 0; i < 8; i++)
    pathObj->addNodeToPath({ 700, 200, -185.722168 }, { -0.000431, -0.267499, 0.963558 });
  for (int i = 0; i < 18; i++)
    pathObj->addNodeToPath({ 0, 500, 0 }, { -0.000431, -0.267499, 0.963558 });
  /*pathObj->addNodeToPath({ B }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(B, C, 0.25f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(B, C, 0.5f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(B, C, 0.75f) }, { 0, 0, 0 });

  pathObj->addNodeToPath({ C }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(C, D, 0.25f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(C, D, 0.5f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(C, D, 0.75f) }, { 0, 0, 0 });

  pathObj->addNodeToPath({ D }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(D, A, 0.25f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(D, A, 0.5f) }, { 0, 0, 0 });
  pathObj->addNodeToPath({ glm::mix(D, A, 0.75f) }, { 0, 0, 0 });
  addUpperCircle(pathObj);*/

	pointlights.push_back(light0);
	pointlights.push_back(light1);
	pointlights.push_back(light2);
	//pointlights.push_back(light3);

	for (auto light : pointlights)
	{
    if (light->hasPath())
		  light->genTrail();
	}
}

int main()
{
  TestApp app;
  app.run();
  return 0;
}