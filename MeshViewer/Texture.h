#pragma once
#include<GL/glut.h>
#include<string>
#include<iostream>
#include"stb_image.h"

using namespace std;

class Texture
{
public:
	GLuint textCode;

	GLubyte* (*makeImageByPixel)(int,int,int);

	Texture() {}

	Texture(string const& fileName)
	{
		loadFromFile(fileName.c_str());
	}

	void BindTexture()
	{
		glBindTexture(GL_TEXTURE_2D, textCode);
	}

	void loadFromMemory(GLubyte* (*makeImageByPixel)(int,int,int), int const& width, int const& height, int const& channel)
	{
		glGenTextures(1, &textCode);
		cout << "Texture: 当前纹理: " << textCode << endl;
		glBindTexture(GL_TEXTURE_2D, textCode);
		// 为当前绑定的纹理对象设置环绕、过滤方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		GLubyte* data = makeImageByPixel(width, height, channel);
		if (data) {
			if (channel == 4) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
		}
		else {
			std::cout << "Failed to load texture" << std::endl;
		}
		free(data);
	}

	void loadFromFile(const char* texturePath)
	{
		glGenTextures(1, &textCode);
		cout << "Texture: 当前纹理: " << textCode << endl;
		glBindTexture(GL_TEXTURE_2D, textCode);
		// 为当前绑定的纹理对象设置环绕、过滤方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// load texture img
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		GLubyte* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
		if (data) {
			if (nrChannels == 4) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			std::cout << "Load texture successfuly" << std::endl;
		}
		else {
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}
};

