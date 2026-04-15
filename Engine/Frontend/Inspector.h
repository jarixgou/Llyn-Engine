#ifndef INSPECTOR__H
#define INSPECTOR__H
#include <string>
#include <vector>

namespace Llyn
{
	class GameObject;
	class Mesh;
	class DirectLight;
	class PointLight;
	class SpotLight;

	class Inspector
	{
	private:
		GameObject** m_selectedGo;

		std::vector<std::string> m_pathsMaterial;

		static Inspector* s_instance;
	public:
		Inspector();
		~Inspector();

		static Inspector* Get();

		void Draw() const;
	private:
		void DrawTransform() const;
		void DrawMesh(Mesh* _mesh) const;
		void DrawDirectLight(DirectLight* _light) const;
		void DrawPointLight(PointLight* _light) const;
		void DrawSpotLight(SpotLight* _light) const;
		void AddComponent() const;
	};
}

#endif