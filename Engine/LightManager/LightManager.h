#ifndef LIGHT_MANAGER__H
#define LIGHT_MANAGER__H
#include <vector>

namespace Llyn
{
	struct DirectLightData;
	struct PointLightData;
	struct SpotLightData;

	class LightManager
	{
	private:
		unsigned int m_directLightSSBO;
		unsigned int m_pointLightSSBO;
		unsigned int m_spotLightSSBO;

		int m_padding[3];

		bool m_uploadDirectLights;
		std::vector<uint16_t*> m_directLightsId;
		std::vector<DirectLightData> m_directLights;

		bool m_uploadPointLights;
		std::vector<uint16_t*> m_pointLightsId;
		std::vector<PointLightData> m_pointLights;

		bool m_uploadSpotLights;
		std::vector<uint16_t*> m_spotLightsId;
		std::vector<SpotLightData> m_spotLights;

		static LightManager* s_instance;
	public:
		LightManager();
		~LightManager();

		static LightManager* Get();

		void Update();

		void AddDirectLight(DirectLightData* _data, uint16_t* _id);
		void RemoveDirectLight(uint16_t _id);
		void UpdateDirectLight(DirectLightData* _data, uint16_t _id);

		void AddPointLight(PointLightData* _data, uint16_t* _id);
		void RemovePointLight(uint16_t _id);
		void UpdatePointLight(PointLightData* _data, uint16_t _id);

		void AddSpotLight(SpotLightData* _data, uint16_t* _id);
		void RemoveSpotLight(uint16_t _id);
		void UpdateSpotLight(SpotLightData* _data, uint16_t _id);
	private:
		void UploadDirectLights();
		void UploadPointLights();
		void UploadSpotLights();
	};
}

#endif