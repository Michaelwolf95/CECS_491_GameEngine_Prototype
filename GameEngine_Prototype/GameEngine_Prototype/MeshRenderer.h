#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <stb/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"
#include "RenderableObject.h"
#include "Component.h"
#include "GameObject.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

class MeshRenderer: public RenderableObject, public Component
{
	public:
		static Registrar<MeshRenderer> registrar;

		std::vector<Texture> textures_loaded;
		std::vector<Mesh> meshes;
		std::string directory;
		bool gammaCorrection;
		std::string pathToObjModel;

		// mapping name of mesh to its respective material in the mesh renderer
		std::unordered_map<std::string, Material*> MeshToMaterial;
		
		// Constructor
		MeshRenderer(string const &path, Material* m = nullptr, bool gamma = false);

		MeshRenderer();
		// Deconstructor
		~MeshRenderer();

		// Renderable Object functions
		void Setup() override; 
		void Draw() override;

		// Component functions
		void Start() override;
		void Update() override;
		void OnDrawGizmos() override;

		void DrawInspector() override;

	private:
		//Material* _material;
		bool isSetup = false;

		void processNode(aiNode *node, const aiScene *scene);
		Mesh processMesh(aiMesh *mesh, const aiScene *scene);
		vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
		unsigned int TextureFromFile(const char * path, const string &directory, bool gamma = false);

		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER()
		template<class Archive>
		void save(Archive & ar, const unsigned int version) const
		{
			//// invoke serialization of the base class 
			ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Component);
			ar & BOOST_SERIALIZATION_NVP(pathToObjModel);
			ar & BOOST_SERIALIZATION_NVP(material);

		}
		template<class Archive>
		void load(Archive & ar, const unsigned int version)
		{
			// invoke serialization of the base class 
			ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Component);
			ar & BOOST_SERIALIZATION_NVP(pathToObjModel);
			ar & BOOST_SERIALIZATION_NVP(material);
			Setup();
		}
};