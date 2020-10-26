#pragma once
#include"Texture.h"
#include<vector>
#include<map>

class AssetManager
{
public:

	static AssetManager* getInstance()
	{
		if (instance == nullptr)
		{
			instance = new AssetManager();
		}
		return instance;
	}

	Texture* getTexture(string const& fileName)
	{
		if (textures.find(fileName) == textures.end())
		{
			textures[fileName] = Texture(fileName);
		}
		return &textures[fileName];
	}

private:

	static AssetManager* instance;

	AssetManager() {}

	map<string, Texture> textures;

};

