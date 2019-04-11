#pragma once

namespace Haze 
{
	class Camera;
	class Scene;
}

namespace GUI
{

	double Sample();

	void FixPath(char* data);

	void BigSeparator();
	void BigSpace();

	void Test();

	void ScriptWindow(bool& show, std::array<char, 1000>& content, bool& execute);

	void RepositoryWindow(bool& show);
	void CameraWindow(bool& show, Haze::Camera* camera);

	void InsertObjectWindow(bool& show, Haze::Scene* scene);
	void SetSkyboxWindow(bool& show, Haze::Scene* scene);

	void ObjectManagerWindow(bool& show, bool& show_insertobject, Haze::Scene* scene);
	void LightManagerWindow(bool& show, Haze::Scene* scene, Haze::Camera* camera);
}