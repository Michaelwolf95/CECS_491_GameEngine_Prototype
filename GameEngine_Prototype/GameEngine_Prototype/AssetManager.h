#pragma once
#include "Singleton.h"
#include "TextureLibrary.h"
#include "ModelLibrary.h"
#include "MaterialLibrary.h"
//class MaterialLibrary;
#include "MeshLibrary.h"
#include "ShaderLibrary.h"
//#include <stb/stb_image.h>

static const char* ASSET_FILE_PATH
#ifdef _WIN32
= "..\\Assets\\";
#else
= "../Assets/";
#endif

class AssetManager : public Singleton<AssetManager>
{
	friend class Singleton<AssetManager>;
public:
	TextureLibrary textureLib;
	ModelLibrary modelLib;
	MaterialLibrary materialLib;
	MeshLibrary meshLib;
	ShaderLibrary shaderLib;
	AssetManager();
	~AssetManager();
	static AssetManager* CreateManager();
	void Init();

	static void LoadTextureAsset(const char * textureFilePath, unsigned int * textureID, unsigned int loadMode = 3);
	static void LoadTexture(const char * textureFilePath, unsigned int * textureID, unsigned int loadMode = 3);
};

