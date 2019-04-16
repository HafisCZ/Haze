#include "hzpch.h"
#include "Texture.h"

#include "Haze/Repository.h"

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
		if (texture == nullptr) 
		{
			return 0;
		}

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

	void TextureCube::Bind() {
		_TotalBinds++;
		glBindTexture(GL_TEXTURE_CUBE_MAP, _Handle);
	}

	Texture* TextureLoader::LoadImpl(const std::string& path)
	{
		Texture* texture = new Texture2D();
		void* buffer = stbi_load(path.c_str(), &texture->_Width, &texture->_Height, &texture->_PPM, 0);
		
		if (buffer != nullptr)
		{
			texture->_Format = (texture->_PPM == 1 ? GL_RED : (texture->_PPM == 3 ? GL_RGB : GL_RGBA));

			glBindTexture(GL_TEXTURE_2D, texture->_Handle);

			glTexImage2D(GL_TEXTURE_2D, 0, texture->_Format, texture->_Width, texture->_Height, 0, texture->_Format, GL_UNSIGNED_BYTE, buffer);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			stbi_image_free(buffer);

			return texture;
		} 
		else 
		{
			HZ_CORE_ERROR("Texture loader error: {0}", stbi_failure_reason());
			return nullptr;
		}
	}

	Texture* TextureLoader::LoadCubeImpl(const std::string& path)
	{
		Texture* texture = new TextureCube();

		std::vector<std::string> textures;
		std::ifstream in(path);

		if (in.is_open())
		{
			std::string tmp;
			while (in >> tmp) textures.push_back(tmp);

			in.close();

			if (textures.size() == 6) 
			{
				glBindTexture(GL_TEXTURE_CUBE_MAP, texture->_Handle);

				for (unsigned int i = 0; i < 6; i++) 
				{
					int x, y, pp;
					void* buffer = stbi_load(textures[i].c_str(), &x, &y, &pp, 0);
					if (buffer) {
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
						stbi_image_free(buffer);
					} else {
						return nullptr;
					}
				}

				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

				return texture;
			}
		}

		HZ_CORE_ERROR("Texture loader error! Cannot load cube texture.");
		return nullptr;
	}

}