#pragma once
#include <GL/glew.h>

namespace Candy
{
	namespace GL
	{
		class GLCubeMap
		{
		public:
			GLCubeMap();
			~GLCubeMap();
			void bind(int unit) const;
			bool loadFromFile(const char* szPath, bool bSwapRB);
			GLuint GetHandle() const { return handle; };
		private:
			void initVao();

			GLuint handle;
			GLuint vao;
		};
	}
}