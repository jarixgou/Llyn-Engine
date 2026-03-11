#version 460 core

out vec4 FragColor;

in VS_OUT
{
	vec3 FragPos;
	vec3 Normal;
	vec3 Color;
	vec2 TexCoord;
} fsIn;

uniform float uGridSize;

void main()
{
	const float cellLineThickness = 0.01f;
	const float subCellLineThickness = 0.001f;

	const float cellSize = 1.f;
	const float halfCellSize = cellSize * 0.5f;

	const float subCellSize = 0.1f;
	const float halfSubCellSize = subCellSize * 0.5f;

	// Utiliser FragPos.xz au lieu de TexCoord pour une grille infinie
	vec2 worldPos = fsIn.FragPos.xz;

	vec2 cellCoords = mod(worldPos + halfCellSize, cellSize);
	vec2 subCellCoords = mod(worldPos + halfSubCellSize, subCellSize);

	vec2 distanceToCell = abs(cellCoords - halfCellSize);
	vec2 distanceToSubCell = abs(subCellCoords - halfSubCellSize);

	const vec2 d = fwidth(worldPos);
	vec2 adjustedCellLineThickness = 0.5f * (cellLineThickness + d);
	vec2 adjustedSubCellLineThickness = 0.5f * (subCellLineThickness + d);

	vec4 finalColor = vec4(0.f, 0.f, 0.f, 0.f);

	if (any(lessThan(distanceToSubCell, adjustedSubCellLineThickness)))
	{
		finalColor = vec4(0.3f, 0.3f, 0.3f, 1.f);
	}
	if (any(lessThan(distanceToCell, adjustedCellLineThickness)))
	{
		finalColor = vec4(0.6f, 0.6f, 0.6f, 1.f);
	}

	FragColor = finalColor;
}