#include "CompileShader.h"

#include <fstream>
#include <filesystem>
#include <string>
#include <slang/slang.h>
#include <slang/slang-com-ptr.h>
#include <slang/slang-com-helper.h>
#include <slang/slang-gfx.h>

#include "Logger.h"
#include "Utils/Utils.h"

void CompileShaders()
{
	std::string path = "Core/Shaders";

	std::vector<std::string> shaderFiles;
	for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
	{
		if (entry.is_regular_file() && entry.path().extension() == ".slang")
		{
			shaderFiles.emplace_back(entry.path().string());
		}
	}

	Slang::ComPtr<slang::IGlobalSession> globalSession;
	slang_createGlobalSession(SLANG_API_VERSION, globalSession.writeRef());

	slang::SessionDesc sessionDesc = {};
	slang::TargetDesc targetDesc = {};
	targetDesc.format = SLANG_SPIRV;
	targetDesc.profile = globalSession->findProfile("spirv_1_5");

	sessionDesc.targetCount = 1;
	sessionDesc.targets = &targetDesc;

	std::vector<slang::CompilerOptionEntry> options =
	{
		{
			slang::CompilerOptionName::EmitSpirvDirectly,
			{slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr}
		}
	};

	sessionDesc.compilerOptionEntryCount = static_cast<uint32_t>(options.size());
	sessionDesc.compilerOptionEntries = options.data();

	Slang::ComPtr<slang::ISession> session;
	globalSession->createSession(sessionDesc, session.writeRef());

	for (size_t i = 0; i < shaderFiles.size(); ++i)
	{
		Slang::ComPtr<slang::IBlob> diagnostics;
		Slang::ComPtr<slang::IModule> slangModule(
			session->loadModule(shaderFiles[i].c_str(), diagnostics.writeRef()));

		if (diagnostics)
		{
			LOGGER_ERROR(static_cast<const char*>(diagnostics->getBufferPointer()));
		}
		if (!slangModule)
		{
			LOGGER_CRITICAL("Module load failed"); continue;
		}

		std::vector<slang::IComponentType*> components = { slangModule };
		for (SlangInt32 e = 0; e < slangModule->getDefinedEntryPointCount(); ++e)
		{
			Slang::ComPtr<slang::IEntryPoint> ep;
			slangModule->getDefinedEntryPoint(e, ep.writeRef());
			components.push_back(ep);
		}

		Slang::ComPtr<slang::IComponentType> composed;
		session->createCompositeComponentType(components.data(), components.size(),
			composed.writeRef(), diagnostics.writeRef());

		Slang::ComPtr<slang::IComponentType> linked;
		composed->link(linked.writeRef(), diagnostics.writeRef());

		Slang::ComPtr<slang::IBlob> spirv;
		linked->getTargetCode(0, spirv.writeRef(), diagnostics.writeRef());

		std::ofstream spvFile;
		spvFile.open(shaderFiles[i] + ".spv", std::ios::binary);
		spvFile.write(static_cast<const char*>(spirv->getBufferPointer()), spirv->getBufferSize());
		spvFile.close();
	}
}
