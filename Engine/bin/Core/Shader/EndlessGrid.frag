#version 460 core

out vec4 FragColor;

in VS_OUT
{
	vec3 FragPos;
	vec3 Normal;
	vec3 Color;
	vec2 TexCoord;
} fsIn;

uniform vec3 uCamPos;

const float cellLineThickness = 0.01f;
const float subCellLineThickness = 0.001f;
const float cellSize = 1.f;
const float halfCellSize = cellSize * 0.5f;
const float subCellSize = 0.1f;
const float halfSubCellSize = subCellSize * 0.5f;


const float cellFadeStart = 50.f;
const float cellFadeEnd = 100.f;
const float subCellFadeStart = 10.f;  
const float subCellFadeEnd = 30.f;    

float OpacityFallOff();
float CalculateCellLOD(float _distanceToCamera);
float CalculateSubCellLOD(float _distanceToCamera);

void main()
{
	vec2 worldPos = fsIn.FragPos.xz;
	float distanceToCamera = length(fsIn.FragPos.xz - uCamPos.xz);

	vec2 cellCoords = mod(worldPos + halfCellSize, cellSize);
	vec2 subCellCoords = mod(worldPos + halfSubCellSize, subCellSize);

	vec2 distanceToCell = abs(cellCoords - halfCellSize);
	vec2 distanceToSubCell = abs(subCellCoords - halfSubCellSize);

	vec2 d = fwidth(worldPos);
	vec2 adjustedCellLineThickness = 0.5f * (cellLineThickness + d);
	vec2 adjustedSubCellLineThickness = 0.5f * (subCellLineThickness + d);

	vec4 subCellColor = vec4(0.3f, 0.3f, 0.3f, 1.f);
	vec4 cellColor = vec4(0.6f, 0.6f, 0.6f, 1.f);
	
	float subCellMask = float(any(lessThan(distanceToSubCell, adjustedSubCellLineThickness)));
	float cellMask = float(any(lessThan(distanceToCell, adjustedCellLineThickness)));

	float cellLOD = CalculateCellLOD(distanceToCamera);
	float subCellLOD = CalculateSubCellLOD(distanceToCamera);
	
	vec4 finalColor = mix(vec4(0.f), cellColor, cellMask * cellLOD);
	finalColor = mix(vec4(0.f), subCellColor, subCellMask * subCellLOD);
	finalColor = mix(finalColor, cellColor, cellMask);

	finalColor.a *= OpacityFallOff();

	if (finalColor.a <= 0.f)
	{
		discard;
	}

	FragColor = finalColor;
}

float CalculateCellLOD(float _distanceToCamera)
{
	return smoothstep(cellFadeEnd, cellFadeStart, _distanceToCamera);
}

float CalculateSubCellLOD(float _distanceToCamera)
{
	return smoothstep(subCellFadeEnd, subCellFadeStart, _distanceToCamera);
}

float OpacityFallOff()
{
	float distanceToCamera = length(fsIn.FragPos.xz - uCamPos.xz);
	float heightToFadeDistanceRatio = 25.f;
	float fadeDistance = clamp(abs(uCamPos.y) * heightToFadeDistanceRatio, 5.f, 50.f);

	return smoothstep(1.0f, 0.0f, distanceToCamera / fadeDistance);
}