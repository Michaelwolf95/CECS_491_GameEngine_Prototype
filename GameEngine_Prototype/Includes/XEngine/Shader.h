#pragma once
#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	// the program ID
	unsigned int ID;
	std::string vFilePath; // TODO: used by material retrieve file for parsing defaults

	// constructor reads and builds the shader
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
	// Single shader file
	Shader(const std::string shaderFilePath);
	// use/activate the shader
	void use();
	// utility uniform functions
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setColor(const std::string &name, float r, float g, float b, float a) const;
	void setVec2(const std::string & name, const glm::vec2 & value) const;
	void setVec2(const std::string & name, float x, float y) const;
	void setVec3(const std::string & name, const glm::vec3 & value) const;
	void setVec3(const std::string & name, float x, float y, float z) const;
	void setVec4(const std::string & name, const glm::vec4 & value) const;
	void setVec4(const std::string & name, float x, float y, float z, float w);
	void setMat2(const std::string & name, const glm::mat2 & mat) const;
	void setMat3(const std::string & name, const glm::mat3 & mat) const;
	void setMat4(const std::string & name, const glm::mat4 & mat) const;
	std::string to_string();
private:
	void checkCompileErrors(unsigned int shader, std::string type);
	int parseShaderFile(const std::string & path);
	void compileShaders(const char* vertexCode, const char* fragmentCode, const char* geometryCode = nullptr);
};

