#include "hzpch.h"
#include "Texture.h"

#include <glad/glad.h>

#include "Haze/STB/stb_image.h"

namespace Haze 
{

	TextureAllocator* TextureAllocator::_Instance = new TextureAllocator(8, 32);

	TextureAllocator::TextureAllocator(unsigned int offset, unsigned int slots) 
	{
		for (unsigned int i = 0; i < slots; i++)
		{
			_SlotAllocations[i + offset] = nullptr;
		}
	}

	void TextureAllocator::FlushImpl() 
	{
		for (auto& it : _SlotLocks)
		{
			it.second = false;
		}
	}

	unsigned int TextureAllocator::BindImpl(Texture* texture) 
	{
		unsigned int selected = 0;
		unsigned long minBindings = -1;

		for (auto& it : _SlotAllocations) 
		{
			if (it.second == texture) return it.first;
			if (_SlotLocks[it.first]) continue;
			if (!it.second)
			{
				selected = it.first;
				break;
			}
			if (it.second->_TotalBinds < minBindings)
			{
				selected = it.first;
				minBindings = it.second->_TotalBinds;
			}
		}

		_SlotAllocations[selected] = texture;
		_SlotLocks[selected] = true;

		glActiveTexture(GL_TEXTURE0 + selected);
		texture->Bind();

		return selected;
	}

	Texture::Texture() 
	{
		glGenTextures(1, &_Handle);
	}

	Texture::~Texture() 
	{
		glDeleteTextures(1, &_Handle);
	}

	void Texture2D::Bind()
	{
		_TotalBinds++;
		glBindTexture(GL_TEXTURE_2D, _Handle);
	}

	Texture* TextureLoader::Load(const std::string& path) 
	{
		Texture* texture = new Texture2D();
		void* buffer = stbi_load(path.c_str(), &texture->_Width, &texture->_Height, &texture->_PPM, 0);

		if (buffer)
		{
			texture->_Format = (texture->_Format == 1 ? GL_RED : (texture->_Format == 3 ? GL_RGB : GL_RGBA));

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture->_Handle);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->_Width, texture->_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);

			stbi_image_free(buffer);

			return texture;
		} 
		else 
		{
			return nullptr;
		}
	}

}