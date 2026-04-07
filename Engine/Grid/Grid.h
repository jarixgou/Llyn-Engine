#ifndef GRID__H
#define GRID__H

namespace Llyn
{
	class Camera;
	class Shader;
	class MeshFilter;

	class Grid
	{
	private:
		Shader* m_shader;
		MeshFilter* m_meshFilter;
	public:
		Grid();
		~Grid() = default;

		void Draw(Camera* _cam);
	};
}

#endif