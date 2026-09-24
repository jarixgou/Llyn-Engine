#ifndef LLYN_CORE__H
#define LLYN_CORE__H

#include <string>

#define GPU_ALIGN alignas(16)
#define ARRAY_SIZE_IN_BYTES(a) (sizeof((a)[0]) * (a).size())

#define MAX_FRAMES_IN_FLIGHT 2

#define LLYN_ASSERT(x) \
	if (!(x)) \
	{\
		\
	}

#define VK_CHECK(x, y) VkCheck(x, y)

#define GET_TYPE_SIGNATURE(T) GetTypeSignature<T>()

#define DEFAULT_ENTITY_NAME "GameObject"

// Vulkan forward declaration
class VK_Device;
class VK_SwapChain;
class VK_FrameSync;
class VK_Instance;
class VK_Pipeline;
class VK_DepthResources;
class VK_DescriptorManager;
class VK_ImGui;
struct VK_ShaderBuffer;
struct VK_Buffer;
struct VK_LayoutInfo;
struct VK_DescriptorLayoutHandle;
class VK_RenderWindow;

// Asset forward declaration
struct TextureConfig;
class Texture;
class Material;

// Component forward declaration
struct Transform;
struct DirLight;
struct LightData;
struct PointLight;
struct SpotLight;
class Camera;
class MeshFilter;
class MeshRender;

class LightManager;

struct PipelineInfo;
typedef uint32_t Index;

struct MetaData;
struct MatricesData;

enum class RenderFace : uint8_t;

class IRender;
class IUniformManager;
struct Scene;
class ECS;
struct Entity;
class Clock;

typedef uint32_t StringHash;
typedef uint16_t RessourceID;
typedef uint32_t EntityID;
typedef uint32_t ComponentID;

#define TOMBSTONE_COMPONENT (std::numeric_limits<ComponentID>::max)()
#define TOMBSTONE_ENTITY (std::numeric_limits<EntityID>::max)()
#define TOMBSTONE_RESOURCE (std::numeric_limits<RessourceID>::max)()

template <typename T>
StringHash GetTypeSignature()
{
	std::string_view sign;
#ifdef _MSC_VER
	sign = __FUNCSIG__;
#elif __clang__ || __GNUC__
	sign = __PRETTY_FUNCTION__;
#else
#error "No supported compilator" 
#endif

	// FNV-1a hash algorithm
	uint32_t hash = 2166136261u;
	constexpr uint32_t hashPrime = 16777619u;
	for (char data : sign)
	{
		hash ^= static_cast<uint32_t>(data);
		hash *= hashPrime;
	}

	return hash;
}

void VkCheck(int _result, std::string _message);

#endif