#pragma once

#include "Haze/Scene/Scene.h"
#include "Haze/Scene/Camera.h"
#include "Program.h"

namespace Haze 
{

	class ProgramAdapter
	{
		public:
			ProgramAdapter(Program* program) : _Program(program) {}

			virtual void Set(Scene* scene, Camera* camera) = 0;
			virtual void Set(Object* object, Camera* camera) = 0;
			virtual void Set(Mesh* mesh, Camera* camera) = 0;

			virtual void OnFinish() {}

			inline void Bind() { _Program->Bind(); }
			inline Program* GetProgram() { return _Program; }

		protected:
			Program* _Program;
	};

	class GeometryPassAdapter : public ProgramAdapter
	{
		public:
			GeometryPassAdapter(Program* program) : ProgramAdapter(program) {}

			virtual void Set(Scene* scene, Camera* camera) override;
			virtual void Set(Object* object, Camera* camera) override;
			virtual void Set(Mesh* mesh, Camera* camera) override;
	};

	class ShadingPassAdapter : public ProgramAdapter {
		public:
			ShadingPassAdapter(Program* program) : ProgramAdapter(program) {}

			virtual void Set(Scene* scene, Camera* camera) override;
			virtual void Set(Object* object, Camera* camera) override;
			virtual void Set(Mesh* mesh, Camera* camera) override;
	};

	class LightingPassAdapter : public ProgramAdapter {
		public:
			LightingPassAdapter(Program* program) : ProgramAdapter(program) {}

			virtual void Set(Scene* scene, Camera* camera) override;
			virtual void Set(Object* object, Camera* camera) override;
			virtual void Set(Mesh* mesh, Camera* camera) override;
	};

	class SkyboxAdapter : public ProgramAdapter {
		public:
			SkyboxAdapter(Program* program) : ProgramAdapter(program) {}

			virtual void Set(Scene* scene, Camera* camera) override;
			virtual void Set(Object* object, Camera* camera) override;
			virtual void Set(Mesh* mesh, Camera* camera) override;
			virtual void OnFinish() override;
	};

	class ForwardPassAdapter : public ProgramAdapter {
		public:
			ForwardPassAdapter(Program* program) : ProgramAdapter(program) {}

			virtual void Set(Scene* scene, Camera* camera) override;
			virtual void Set(Object* object, Camera* camera) override;
			virtual void Set(Mesh* mesh, Camera* camera) override;
	};

}