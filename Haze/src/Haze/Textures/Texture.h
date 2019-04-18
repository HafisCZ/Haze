#pragma once

#include "Haze/Core.h"
#include "Haze/Repository.h"

#include <string>

namespace Haze 
{

	class Texture
	{
		friend class TextureLoader;
		friend class TextureAllocator;

		friend class RendererLayer;

		friend struct GUI;

		public:
			Texture();
			virtual ~Texture();

		private:
			virtual void Bind() = 0;

		protected:
			unsigned long _TotalBinds = 0;
			unsigned int _Handle;

			int _Width = -1;
			int _Height = -1;
			int _PPM = -1;
			int _Format = -1;
	};

	class TextureAllocator
	{
		public:
			TextureAllocator(unsigned int offset, unsigned int slots);
			
			inline static unsigned int Bind(Texture* texture) { return _Instance->BindImpl(texture); }
			inline static void Flush() { _Instance->FlushImpl(); }

		private:
			unsigned int BindImpl(Texture* texture);
			void FlushImpl();

		private:
			std::unordered_map<unsigned int, Texture*> _SlotAllocations;
			std::unordered_map<unsigned int, bool> _SlotLocks;

			static TextureAllocator* _Instance;
	};

	class Texture2D : public Texture
	{
		public:
			Texture2D() : Texture() {}

		private:
			virtual void Bind() override;
	};

	class TextureCube : public Texture
	{
		public:
			TextureCube() : Texture() {}

		private:
			virtual void Bind() override;
	};

	class TextureLoader
	{
		public:
			static Texture* Load(const std::string& path) { return static_cast<Texture*>(Repository::Load(path, lambda::make([&]() -> void* { return LoadImpl(path); }))); }
			static Texture* LoadCube(const std::string& path) { return static_cast<Texture*>(Repository::Load(path, lambda::make([&]() -> void* { return LoadCubeImpl(path); }))); }

		private:
			static Texture* LoadImpl(const std::string& path);
			static Texture* LoadCubeImpl(const std::string& path);
	};
	
}