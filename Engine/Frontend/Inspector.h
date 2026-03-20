#ifndef INSPECTOR__H
#define INSPECTOR__H

namespace Llyn
{
	class GameObject;
	class Mesh;

	class Inspector
	{
	private:
		GameObject** m_selectedGo;

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