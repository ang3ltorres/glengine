#pragma once

#include "pch.hpp"

namespace graphics
{
	class Shader
	{
	public:
		Shader(const char *vertexShader, const char *fragmentShader);
		~Shader();

		static char infoLog[512];
		static Shader *current;

		void use();
		GLint getUniformLocation(const char *name);
		GLuint program;

	private:
		std::unordered_map<std::string, GLint> uniformCache;
	};
}
