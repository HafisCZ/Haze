#pragma once

namespace Haze
{
	class Camera;
	class Scene;

	struct GUI
	{
		static double Sample();

		static void FixPath(char* data);

		static void BigSeparator();
		static void BigSpace();

		static void ScriptWindow(bool& show, Scene* scene, Camera* camera);

		static void RepositoryWindow(bool& show);
		static void CameraWindow(bool& show, Haze::Camera* camera);

		static void InsertObjectWindow(bool& show, Haze::Scene* scene);
		static void SetSkyboxWindow(bool& show, Haze::Scene* scene);

		static void ObjectManagerWindow(bool& show, bool& show_insertobject, Haze::Scene* scene, Haze::Camera* camera);
		static void LightManagerWindow(bool& show, Haze::Scene* scene, Haze::Camera* camera);

		static void Menu(Scene* scene, Camera* camera, int& drawmode, int& normals);
		static bool InFocus();
	};
}