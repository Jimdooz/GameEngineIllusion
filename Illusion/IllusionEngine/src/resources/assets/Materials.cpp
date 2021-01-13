#include "resources/assets/Materials.h"

#include "core/rendering/Renderer.h"

namespace illusion::resources::assets {

	void LoadAllMaterials(std::string path) {
		RenderEngine& renderEngine = GetRenderEngine();
		auto& materials = renderEngine.materials;
		materials.clear();

		renderEngine.AddMaterial({ 0, 0, "default material", "", "", {
		{ "ambient", {1.0f,1.0f,1.0f,1.0f} },
		{ "diffuse", {1.0f,1.0f,1.0f,1.0f} },
		{ "specular", {1.0f,1.0f,1.0f,1.0f} },
		{ "shininess", 32.0 }
		} }, 0);

		if (illusion::resources::CurrentProject().path == "") return;
		LoadAllMaterialsRecursive(path);
	}

	void LoadAllMaterialsRecursive(std::string path) {
		RenderEngine& renderEngine = GetRenderEngine();

		for (auto& p : fs::directory_iterator(path)) {
			std::string currPath = p.path().filename().string();
			if (p.is_directory() && currPath.find('.') == std::string::npos) {
				LoadAllMaterialsRecursive(path + "/" + currPath);
			}
			else if (currPath.find(".material") != std::string::npos) {
				//LOAD MATERIAL
				try {
					MaterialResource material = LoadMaterial(path + "/" + currPath);
					renderEngine.AddMaterial(material, material.id);
				}
				catch (const std::exception& exception) {
					INTERNAL_ERR("Impossible de lire le material ", path + "/" + currPath, "\n", exception.what());
				}
			}
		}
	}

}