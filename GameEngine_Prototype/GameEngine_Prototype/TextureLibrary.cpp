#include "TextureLibrary.h"
#include "AssetManager.h"
#include "Texture.h"

TextureLibrary::TextureLibrary() {}

TextureLibrary::~TextureLibrary() {}

// Checks texture library for existing texture, if exists, no new texture is created, if not, stores a new texture.
// Calls AssetManager::LoadTextureAsset to load texture into memory and assign texture ID.
Texture & TextureLibrary::LoadAsset(std::string filePath)
{
	Texture* text = new Texture();
	text->path = filePath;

	library.insert({ filePath, *text });

	// if does not contain the directory 'Assets'
	std::string fullPath;
	if (filePath.find("Assets") == std::string::npos)
		fullPath = ASSET_FILE_PATH + std::string(filePath);
	else
		fullPath = std::string(filePath);

	AssetManager::LoadTextureAsset(fullPath.c_str(), &library[filePath].id); // <-- Maybe this should be in this class too, instead of just in asset manager.
	return library[filePath];
}

/*
// TODO: Move this to TextureLibrary..?
unsigned int TextureLibrary::TextureFromFile(const char * path, const std::string &directory, bool gamma)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

	if (data)
	{
		GLenum format;

		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
	return textureID;
}
*/