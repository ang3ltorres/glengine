#pragma once

#include "pch.hpp"

namespace gui
{
	class Shader
	{
	public:
		Shader(const char *vertexShader, const char *fragmentShader);
		~Shader();

		static char infoLog[512];
		static Shader *current;

		void use();
		GLuint program;
	};
}
