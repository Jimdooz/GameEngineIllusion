#pragma once
#include <streambuf>
#include <sstream>

#include <fstream>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

#include "core/rendering/Renderer.h"
#include "resources/Project.h"
#include "core/rendering/Importer.h"

#include "resources/DataConvertor.h"
#include "resources/system/Json.h"
using json = illusion::json;

#include "core/rendering/shapes/defaultShapes.h"

namespace illusion::resources::assets {

	using namespace illusion;

	void LoadAllMeshes(std::string path = CurrentProject().path + "/Assets");
	void LoadAllMeshesRecursive(std::string path = CurrentProject().path + "/Assets");
}