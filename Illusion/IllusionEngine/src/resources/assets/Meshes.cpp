#include "resources/assets/Meshes.h"

#include "core/rendering/Renderer.h"

namespace illusion::resources::assets {

	void LoadAllMeshes(std::string path) {
		RenderEngine& renderEngine = GetRenderEngine();
		auto& meshes = renderEngine.meshes;

		for (auto& [meshId, mesh] : GetRenderEngine().meshes) {
			mesh.ClearOnGPU();
		}
		meshes.clear();

		//Default Mesh
		renderEngine.AddMesh(illusion::defaultshape::Cube(), 0);
		renderEngine.AddMesh(illusion::defaultshape::IcoSphere(), 1);
		renderEngine.AddMesh(illusion::defaultshape::Quad(), 2);

		if (illusion::resources::CurrentProject().path == "") return;
		LoadAllMeshesRecursive(path);
	}

	void LoadAllMeshesRecursive(std::string path) {
		for (auto& p : fs::directory_iterator(path)) {
			std::string currPath = p.path().filename().string();
			if (p.is_directory() && currPath.find('.') == std::string::npos) {
				LoadAllMeshesRecursive(path + "/" + currPath);
			}
			else if (currPath.find(".fbx") != std::string::npos || currPath.find(".obj") != std::string::npos || currPath.find(".dae") != std::string::npos) {
				//LOAD MESHES
				try {
					illusion::load3DModel((path + "/" + currPath).c_str());
				}
				catch (const std::exception& exception) {
					INTERNAL_ERR("Impossible de lire le mesh ", path + "/" + currPath, "\n", exception.what());
				}
			}
		}
	}

}