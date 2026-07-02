#include <cstdlib>

#if defined(ARCLIGHT_HAS_SDL3)
#include <SDL3/SDL.h>
#endif

#if defined(ARCLIGHT_HAS_GLM)
#include <glm/glm.hpp>
#endif

#if defined(ARCLIGHT_HAS_ENTT)
#include <entt/entt.hpp>
#endif

#if defined(ARCLIGHT_HAS_IMGUI)
#include <imgui.h>
#endif

#if defined(ARCLIGHT_HAS_VULKAN)
#include <vulkan/vulkan.h>
#endif

#if defined(_WIN32) && defined(ARCLIGHT_HAS_D3D12)
#include <d3d12.h>
#include <dxgi1_6.h>
#endif

struct Transform {
#if defined(ARCLIGHT_HAS_GLM)
	glm::vec3 position{};
#else
	float position[3]{};
#endif
};

int main(int, char**)
{
#if defined(ARCLIGHT_HAS_SDL3)
	SDL_SetMainReady();
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		return EXIT_FAILURE;
	}
	SDL_Quit();
#endif

#if defined(ARCLIGHT_HAS_ENTT)
	entt::registry registry;
	const auto entity = registry.create();
	registry.emplace<Transform>(entity);
#endif

#if defined(ARCLIGHT_HAS_IMGUI)
	ImGui::CreateContext();
	ImGui::DestroyContext();
#endif

#if defined(ARCLIGHT_HAS_VULKAN)
	static_assert(VK_API_VERSION_1_0 > 0);
#endif

#if defined(_WIN32) && defined(ARCLIGHT_HAS_D3D12)
	static_assert(D3D12_SDK_VERSION >= 0);
#endif

	return EXIT_SUCCESS;
}
