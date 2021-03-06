//#include <glad/glad.h>
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <stb/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <boost/serialization/unordered_map.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "RenderableObject.h"
#include "Component.h"
#include "GameObject.h"
#include "Model.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

namespace XEngine  {
class ENGINE_API MeshRenderer : public RenderableObject, public Component
{
	public:
		static Registrar<MeshRenderer> registrar;
		//std::string pathToObjModel;
		std::string meshPath; //ex:  ...nanosuit.obj|head
		std::string materialPath;

		Mesh* mesh = nullptr;
		Material* material = nullptr;

		// Constructor
		MeshRenderer(std::string const &modelPath, std::string materialPath = "", bool gamma = false);

		MeshRenderer();
		// Deconstructor
		~MeshRenderer();

		// Renderable Object functions
		void Setup() override;
		void Draw() override;

		// Component functions
		void Start() override;
		void Update() override;
		void OnEnable() override;
		void OnDisable() override;
		void OnDrawGizmos() override;

		void DrawInspector() override;

		//void FreeObjectResources();
		//void FreeAllResources()
		//void PrintVertices();

	private:
		//Material* _material;
		bool isSetup = false;

		//bool LoadModel();
		//void Load();


		//void processNode(aiNode *node, const aiScene *scene);
		//Mesh* processMesh(aiMesh *mesh, const aiScene *scene);
		//std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
		//unsigned int TextureFromFile(const char * path, const std::string &directory, bool gamma = false);


		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER()
		template<class Archive>
		void save(Archive & ar, const unsigned int version) const
		{
			//// invoke serialization of the base class 
			ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Component);
			ar & BOOST_SERIALIZATION_NVP(meshPath);
			ar & BOOST_SERIALIZATION_NVP(materialPath);
		}
		template<class Archive>
		void load(Archive & ar, const unsigned int version)
		{
			// invoke serialization of the base class 
			ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Component);
			ar & BOOST_SERIALIZATION_NVP(meshPath);
			ar & BOOST_SERIALIZATION_NVP(materialPath);

			Setup();
		}
		//unsigned int TextureFromFile(const char * path, const std::string & directory, bool gamma);
};
}