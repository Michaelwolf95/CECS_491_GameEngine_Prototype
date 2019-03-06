#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Serialization.h"

/* TODO: The information we want to store about each material
should be stored in "material modules" that can be attached to it.
For now, we can just use some flags to check if its lit or not.
*/
class Material
{
public:
	std::string vertexShaderPath;
	std::string fragmentShaderPath;
	Shader* shader;
	bool useLight = false;

	std::string textureFilePath;
	unsigned int textureID = 0;
	
	float ambientStrength = 1.0f;
	float specularStrength = 0.5f;
	glm::vec4 Color;
	Material(std::string vertPath, std::string fragPath, bool _useLight = false);
	Material();
	~Material();
	void Init();
	void Load(); // Load for openGL to draw.

	void LoadTexture(const char* _textureFilePath);

private:
	bool isInitialized = false;

	friend class boost::serialization::access;
	//friend std::ostream & operator<<(std::ostream &os, const Material &material);
	BOOST_SERIALIZATION_SPLIT_MEMBER()
	template<class Archive>
	void save(Archive & ar, const unsigned int version) const
	{
		ar & BOOST_SERIALIZATION_NVP(Color);
		ar & BOOST_SERIALIZATION_NVP(useLight);
		ar & BOOST_SERIALIZATION_NVP(vertexShaderPath);
		ar & BOOST_SERIALIZATION_NVP(fragmentShaderPath);
		ar & BOOST_SERIALIZATION_NVP(textureFilePath);
		ar & BOOST_SERIALIZATION_NVP(specularStrength);
		ar & BOOST_SERIALIZATION_NVP(ambientStrength);

	}
	template<class Archive>
	void load(Archive & ar, const unsigned int version) // file_version
	{
		ar & BOOST_SERIALIZATION_NVP(Color);
		ar & BOOST_SERIALIZATION_NVP(useLight);
		ar & BOOST_SERIALIZATION_NVP(vertexShaderPath);
		ar & BOOST_SERIALIZATION_NVP(fragmentShaderPath);
		ar & BOOST_SERIALIZATION_NVP(textureFilePath);
		ar & BOOST_SERIALIZATION_NVP(specularStrength);
		ar & BOOST_SERIALIZATION_NVP(ambientStrength);

		Init();
		//shader = RenderManager::defaultShader;
		
	}

};
//BOOST_CLASS_EXPORT

//std::ostream & operator<<(std::ostream &os, const Material &material);
//BOOST_SERIALIZATION_