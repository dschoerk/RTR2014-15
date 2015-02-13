#include "GLProgram.h"

Candy::GL::GLProgram::GLProgram() : primitive_mode(PrimitiveMode::TRIANGLES)
{
  
}

void Candy::GL::GLProgram::initialize()
{
  handle = glCreateProgram();
}

void Candy::GL::GLProgram::attachShader(GLShader const& shader)
{
  glAttachShader(handle, shader.handle);
}

void Candy::GL::GLProgram::link()
{
  
  glLinkProgram(handle);

  // Check the compilation status and report any errors
  GLint shaderStatus = GL_TRUE;
  glGetShaderiv(handle, GL_LINK_STATUS, &shaderStatus);

  // If the shader failed to compile, display the info log and quit out
  if (shaderStatus == GL_FALSE)
  {
    GLint infoLogLength = 0;
    glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::cout << "fail " << infoLogLength << std::endl;

    if (infoLogLength > 0)
    {
      GLchar *strInfoLog = new GLchar[infoLogLength];
      glGetShaderInfoLog(handle, infoLogLength, nullptr, strInfoLog);

      std::cout << " shader linking failed: " << std::string(strInfoLog, infoLogLength) << std::endl;
      delete[] strInfoLog;
    }
    else
      std::cout << " shader linking failed: no message" << std::endl;
  }
  else
  {
    std::cout << " shader linking OK" << std::endl;
  }


}

void Candy::GL::GLProgram::bind()
{
  glUseProgram(handle);
}

int Candy::GL::GLProgram::getNumberOfAttributes()
{
  GLint param;
  glGetProgramInterfaceiv(handle, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &param);
  return param;
}

int Candy::GL::GLProgram::getNumberOfUniforms()
{
  GLint param;
  glGetProgramInterfaceiv(handle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &param);
  return param;
}

int Candy::GL::GLProgram::getAttributeByName(char const* name)
{
  return glGetProgramResourceIndex(handle, GL_PROGRAM_INPUT, name);
}

int Candy::GL::GLProgram::getUniformByName(char const* name)
{
  int loc;
  if (uniformCache.find(name) == uniformCache.end())
  {
    loc = glGetUniformLocation(handle, name);
    std::cout << "get Uniform " << name << std::endl;
    uniformCache[name] = loc;
  }
  else
  {
    loc = uniformCache[name];
  }

  return loc;
}

Candy::PrimitiveMode Candy::GL::GLProgram::getPrimitiveMode()
{
  return primitive_mode;
}

void Candy::GL::GLProgram::setUniform(char const* name, float u)
{
  int loc = getUniformByName(name);
  //assert(loc > -1);
  glUniform1f(loc, u);
}

void Candy::GL::GLProgram::setUniform(char const* name, int u)
{
  int loc = getUniformByName(name);
  //assert(loc > -1);
  glUniform1i(loc, u);
}

void Candy::GL::GLProgram::setUniform(const char* name, const glm::vec2& u)
{
  int loc = getUniformByName(name);
  //assert(loc > -1);
  glUniform2f(loc, u[0], u[1]);
}

void Candy::GL::GLProgram::setUniform(const char* name, const glm::vec3& u)
{
  int loc = getUniformByName(name);
  //assert(loc > -1);
  glUniform3f(loc, u[0], u[1], u[2]);
}

void Candy::GL::GLProgram::setUniform(const char* name, const glm::vec4& u)
{
  int loc = getUniformByName(name);
  //assert(loc > -1);
  glUniform4f(loc, u[0], u[1], u[2], u[3]);
}

void Candy::GL::GLProgram::setUniform(const char* name, const glm::mat3& u)
{
  int loc = getUniformByName(name);
  //assert(loc > -1);
  glUniformMatrix3fv(loc, 1, GL_FALSE, &u[0][0]);
}

void Candy::GL::GLProgram::setUniform(const char* name, const glm::mat4& u)
{
  int loc = getUniformByName(name);
  //assert(loc > -1);
  glUniformMatrix4fv(loc, 1, GL_FALSE, &u[0][0]);
}

void Candy::GL::GLProgram::setUniform(const char* name, const std::vector<glm::mat4>& u)
{
  int loc = getUniformByName(name);
  //assert(loc > -1);
  glUniformMatrix4fv(loc, u.size(), GL_FALSE, &u[0][0][0]);
}

void Candy::GL::GLProgram::setUniform(char const* name, GLTexture const* tex, int iTextureSlot)
{
  int loc = getUniformByName(name);
  //assert(loc > -1);
  tex->bind(iTextureSlot);
  glUniform1i(loc, iTextureSlot);
}

void Candy::GL::GLProgram::setPrimitiveMode(PrimitiveMode mode)
{
  primitive_mode = mode;
}

/*void Candy::GL::GLProgram::queryAttributes()
{
  const GLenum properties[3] = { GL_TYPE, GL_NAME_LENGTH, GL_LOCATION };
  const int n = getNumberOfAttributes();
  for (int attr = 0; attr < n; ++attr)
  {
    GLint values[3];
    glGetProgramResourceiv(handle, GL_PROGRAM_INPUT, attr, 3, properties, 3, nullptr, values);
    std::string nameData;
    nameData.resize(values[1]);
    glGetProgramResourceName(handle, GL_PROGRAM_INPUT, attr, nameData.size(), nullptr, &nameData[0]);
    std::string name(nameData.begin(), nameData.end() - 1);

    Attribute attribute;
    attribute.location = values[2];
    attribute.name = name;
    attribute.type = static_cast<FieldType>(values[0]);
    attributes.push_back(attribute);

    std::cout << toString(attribute.type) << std::endl;
  }
}*/