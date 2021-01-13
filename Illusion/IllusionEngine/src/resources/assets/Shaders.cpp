#include "resources/assets/Shaders.h"

#include "core/rendering/Renderer.h"

namespace illusion::resources::assets {

	void LoadAllShaders(std::string path) {
		RenderEngine& renderEngine = GetRenderEngine();
		auto& shaders = renderEngine.shaders;
		shaders.clear();

		renderEngine.AddShader(Shader::defaultShader, 0);

		if (illusion::resources::CurrentProject().path == "") return;
		LoadAllShadersRecursive(path);
	}

	void LoadAllShadersRecursive(std::string path) {
		RenderEngine& renderEngine = GetRenderEngine();

		for (auto& p : fs::directory_iterator(path)) {
			std::string currPath = p.path().filename().string();
			if (p.is_directory() && currPath.find('.') == std::string::npos) {
				LoadAllShadersRecursive(path + "/" + currPath);
			}
			else if (currPath.find(".shader") != std::string::npos) {
				//LOAD SHADER
				try {
					ShaderResource shaderRes = LoadShader(path + "/" + currPath);
					renderEngine.AddShader(Shader(shaderRes), shaderRes.id);
				}
				catch (const std::exception&) {
					INTERNAL_ERR("Impossible de lire le shader ", path + "/" + currPath);
				}
			}
		}
	}

}