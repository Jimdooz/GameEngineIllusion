#pragma once

#include <string>

namespace illusion::resources {

	struct Project {
		bool open = false;
		std::string name = "";
		std::string path = "";
		std::string currentScenePath = "";

		static Project currentProject;
	};

	static inline Project& CurrentProject() {
		return Project::currentProject;
	}
}