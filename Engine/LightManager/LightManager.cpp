#include "LightManager.h"

#include <iostream>
#include <glad/glad.h>

#include "DirectLightData.h"
#include "PointLightData.h"
#include "SpotLightData.h"
#include "../Memory/MemoryHelper.h"

namespace Llyn
{
	LightManager* LightManager::s_instance = nullptr;

	LightManager::LightManager()
	{
		m_uploadDirectLights = true;
		m_uploadPointLights = true;
		m_uploadSpotLights = true;

		for (int i = 0; i < 3; ++i)
		{
			m_padding[i] = 0;
		}

		const uint16_t size = std::numeric_limits<uint16_t>::max();

		// ==== SSBO Directional Lights (binding = 0) ====
		constexpr size_t dirBufferSize = sizeof(int) + sizeof(int) * 3 + size * sizeof(DirectLightData);

		glGenBuffers(1, &m_directLightSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_directLightSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, dirBufferSize, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_directLightSSBO);

		// ==== SSBO Point Lights (binding = 1) ====
		constexpr size_t pointBufferSize = sizeof(int) + sizeof(int) * 3 + size * sizeof(PointLightData);

		glGenBuffers(1, &m_pointLightSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_pointLightSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, pointBufferSize, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_pointLightSSBO);

		// ==== SSBO Spot Lights (binding = 2) ====
		constexpr size_t spotBufferSize = sizeof(int) + sizeof(int) * 3 + size * sizeof(SpotLightData);

		glGenBuffers(1, &m_spotLightSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_spotLightSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, spotBufferSize, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_spotLightSSBO);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	LightManager::~LightManager()
	{
		glDeleteBuffers(1, &m_directLightSSBO);
		glDeleteBuffers(1, &m_pointLightSSBO);
		glDeleteBuffers(1, &m_spotLightSSBO);
	}

	LightManager* LightManager::Get()
	{
		if (s_instance == nullptr)
		{
			ALLOCATE_MEMORY(s_instance);
		}

		return s_instance;
	}

	void LightManager::Update()
	{
		UploadDirectLights();
		UploadPointLights();
		UploadSpotLights();
	}

	void LightManager::AddDirectLight(DirectLightData* _data, uint16_t* _id)
	{
		if (_id == nullptr)
		{
			return;
		}

		if (_data == nullptr)
		{
			*_id = std::numeric_limits<uint16_t>::max();
		}

		m_uploadDirectLights = true;

		m_directLights.emplace_back(*_data);
		
		*_id = m_directLights.size() - 1;
		m_directLightsId.emplace_back(_id);
	}

	void LightManager::RemoveDirectLight(uint16_t _id)
	{
		if (_id >= m_directLights.size())
		{
			return;
		}

		m_directLights.erase(m_directLights.begin() + _id);
		m_directLightsId.erase(m_directLightsId.begin() + _id);
		for (int i = _id; i < m_directLightsId.size(); ++i)
		{
			if (*m_directLightsId[i] > 0)
			{
				*m_directLightsId[i] -= 1;
			}
		}
	}

	void LightManager::UpdateDirectLight(DirectLightData* _data, uint16_t _id)
	{
		if (_data == nullptr || _id >= m_directLights.size())
		{
			return;
		}

		m_directLights[_id] = *_data;

		m_uploadDirectLights = true;
	}

	void LightManager::AddPointLight(PointLightData* _data, uint16_t* _id)
	{
		if (_id == nullptr)
		{
			return;
		}

		if (_data == nullptr)
		{
			*_id = std::numeric_limits<uint16_t>::max();
		}

		m_uploadPointLights = true;

		m_pointLights.emplace_back(*_data);
		*_id = m_pointLights.size() - 1;
		m_pointLightsId.emplace_back(_id);
	}

	void LightManager::RemovePointLight(uint16_t _id)
	{
		if (_id >= m_pointLights.size())
		{
			return;
		}

		m_pointLights.erase(m_pointLights.begin() + _id);
		m_pointLightsId.erase(m_pointLightsId.begin() + _id);
		for (int i = _id; i < m_pointLightsId.size(); ++i)
		{
			if (*m_pointLightsId[i] > 0)
			{
				*m_pointLightsId[i] -= 1;
			}
		}
	}

	void LightManager::UpdatePointLight(PointLightData* _data, uint16_t _id)
	{
		if (_data == nullptr || _id >= m_pointLights.size())
		{
			return;
		}

		m_pointLights[_id] = *_data;

		m_uploadPointLights = true;
	}

	void LightManager::AddSpotLight(SpotLightData* _data, uint16_t* _id)
	{
		if (_id == nullptr)
		{
			return;
		}

		if (_data == nullptr)
		{
			*_id = std::numeric_limits<uint16_t>::max();
		}

		m_uploadSpotLights = true;

		m_spotLights.emplace_back(*_data);
		*_id = m_spotLights.size() - 1;
		m_spotLightsId.emplace_back(_id);
	}

	void LightManager::RemoveSpotLight(uint16_t _id)
	{
		if (_id >= m_spotLights.size())
		{
			return;
		}

		m_spotLights.erase(m_spotLights.begin() + _id);
		m_spotLightsId.erase(m_spotLightsId.begin() + _id);
		for (int i = _id; i < m_spotLightsId.size(); ++i)
		{
			if (*m_spotLightsId[i] > 0)
			{
				*m_spotLightsId[i] -= 1;
			}
		}
	}

	void LightManager::UpdateSpotLight(SpotLightData* _data, uint16_t _id)
	{
		if (_data == nullptr || _id >= m_spotLights.size())
		{
			return;
		}

		m_spotLights[_id] = *_data;

		m_uploadSpotLights = true;
	}

	void LightManager::UploadDirectLights()
	{
		if (!m_uploadDirectLights)
		{
			return;
		}
		m_uploadDirectLights = false;

		int count = static_cast<int>(m_directLights.size());

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_directLightSSBO);

		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int), &count);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(int), sizeof(m_padding), m_padding);

		if (!m_directLights.empty())
		{
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16, count * sizeof(DirectLightData), m_directLights.data());
		}

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void LightManager::UploadPointLights()
	{
		if (!m_uploadPointLights)
		{
			return;
		}
		m_uploadPointLights = false;

		int count = static_cast<int>(m_pointLights.size());

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_pointLightSSBO);

		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int), &count);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(int), sizeof(m_padding), m_padding);

		if (!m_pointLights.empty())
		{
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16, count * sizeof(PointLightData), m_pointLights.data());
		}

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void LightManager::UploadSpotLights()
	{
		if (!m_uploadSpotLights)
		{
			return;
		}
		m_uploadSpotLights = false;

		int count = static_cast<int>(m_spotLights.size());

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_spotLightSSBO);

		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int), &count);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(int), sizeof(m_padding), m_padding);

		if (!m_directLights.empty())
		{
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 16, count * sizeof(SpotLightData), m_spotLights.data());
		}

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}
