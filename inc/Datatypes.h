#pragma once

#include <gl/glew.h>
#include <string>

namespace Candy
{
  enum AttributeUsage
  {
    POSITION, NORMAL, COLOR, TEXCOORD2D 
  };

  enum FieldType
  {
    BYTE = GL_BYTE,
    UBYTE = GL_UNSIGNED_BYTE,
    SHORT = GL_SHORT,
    USHORT = GL_UNSIGNED_SHORT,
    INT = GL_INT,
    UINT = GL_UNSIGNED_INT,
    FLOAT = GL_FLOAT,
    DOUBLE = GL_DOUBLE,
    FLOAT_VEC2 = GL_FLOAT_VEC2,
    FLOAT_VEC3 = GL_FLOAT_VEC3,
    FLOAT_VEC4 = GL_FLOAT_VEC4,
    INT_VEC2 = GL_INT_VEC2,
    INT_VEC3 = GL_INT_VEC3,
    INT_VEC4 = GL_INT_VEC4,
    UINT_VEC2 = GL_UNSIGNED_INT_VEC2,
    UINT_VEC3 = GL_UNSIGNED_INT_VEC3,
    UINT_VEC4 = GL_UNSIGNED_INT_VEC4
  };

  inline GLuint basetype(FieldType field_type)
  {
    GLuint type = -1;
    switch (field_type)
    {
    case FLOAT_VEC2: type = GL_FLOAT; break;
    case FLOAT_VEC3: type = GL_FLOAT; break;
    case FLOAT_VEC4: type = GL_FLOAT; break;
    case INT_VEC2: type = GL_INT; break;
    case INT_VEC3: type = GL_INT; break;
    case INT_VEC4: type = GL_INT; break;
    case UINT_VEC2: type = GL_UNSIGNED_INT; break;
    case UINT_VEC3: type = GL_UNSIGNED_INT; break;
    case UINT_VEC4: type = GL_UNSIGNED_INT; break;
    default: type = field_type;
    }

    return type;
  }

  inline int components(FieldType t)
  {
    int comp;
    switch (t)
    {
    case FLOAT_VEC2: comp = 2; break;
    case FLOAT_VEC3: comp = 3; break;
    case FLOAT_VEC4: comp = 4; break;
    case INT_VEC2: comp = 2; break;
    case INT_VEC3: comp = 3; break;
    case INT_VEC4: comp = 4; break;
    case UINT_VEC2: comp = 2; break;
    case UINT_VEC3: comp = 3; break;
    case UINT_VEC4: comp = 4; break;
    default: comp = 0;
    }

    return comp;
  }

  inline bool isInt(FieldType t)
  {
    switch (t)
    {
    case FLOAT_VEC2: return false;
    case FLOAT_VEC3: return false;
    case FLOAT_VEC4: return false;
    case INT_VEC2: return true;
    case INT_VEC3: return true;
    case INT_VEC4: return true;
    case UINT_VEC2: return true;
    case UINT_VEC3: return true;
    case UINT_VEC4: return true;
    default: return false;
    }
  }

  inline std::string toString(FieldType t)
  {
    std::string name;
    switch (t)
    {
    case BYTE: name = "BYTE"; break;
    case UBYTE: name = "UNSIGNED BYTE"; break;
    case SHORT: name = "SHORT"; break;
    case USHORT: name = "UNSIGNED SHORT"; break;
    case INT: name = "INT"; break;
    case UINT: name = "UNSIGNED INT"; break;
    case FLOAT: name = "FLOAT"; break;
    case DOUBLE: name = "DOUBLE"; break;
    case FLOAT_VEC2: name = "FLOAT_VEC2"; break;
    case FLOAT_VEC3: name = "FLOAT_VEC3"; break;
    case FLOAT_VEC4: name = "FLOAT_VEC4"; break;
    case INT_VEC2: name = "INT_VEC2"; break;
    case INT_VEC3: name = "INT_VEC3"; break;
    case INT_VEC4: name = "INT_VEC4"; break;

    default: name = "unknown type";
    }
    return name;
  }

  inline GLuint byteSize(FieldType t)
  {
    GLuint s;
    switch (t)
    {
    case BYTE: s = 1; break;
    case UBYTE: s = 1; break;
    case SHORT: s = 2; break;
    case USHORT: s = 2; break;
    case INT: s = 4; break;
    case UINT: s = 4; break;
    case FLOAT: s = 4; break;
    case DOUBLE: s = 8; break;
    case FLOAT_VEC2: s = 8; break;
    case FLOAT_VEC3: s = 12; break;
    case FLOAT_VEC4: s = 16; break;
    case INT_VEC2: s = 8; break;
    case INT_VEC3: s = 12; break;
    case INT_VEC4: s = 16; break;
    default: s = 0;
    }
    return s;
  }
}