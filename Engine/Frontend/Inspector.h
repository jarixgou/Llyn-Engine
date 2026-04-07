#ifndef INSPECTOR__H
#define INSPECTOR__H
#include <string>
#include <vector>

namespace Llyn
{
	class GameObject;
	class Mesh;

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
	};
}

#endif