#pragma once

#include <fstream>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

#include "project/ProjectManager.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace illusioneditor::views::FileExplorer {

	namespace {
		std::filesystem::directory_entry currentPath("C:\\");
		std::string pathName = currentPath.path().string();
		std::string projectName = "";
	}

	void Show() {
		ImGui::Begin("File Explorer");

		ImGui::Button("..");
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
			currentPath = std::filesystem::directory_entry(currentPath.path().parent_path());
			pathName = currentPath.path().string();
		}
		ImGui::SameLine();
		size_t n = pathName.length() + 64;
		char* buf1 = new char[n];
		strcpy(buf1, pathName.c_str());
		ImGui::InputText("###goto", buf1, n);

		pathName = std::string(buf1);
		delete[] buf1;

		ImGui::SameLine();
		if (ImGui::Button("Go")) {
			currentPath = std::filesystem::directory_entry(pathName);
		}

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::BeginChild("##ScrollingRegion", ImVec2(0, ImGui::GetWindowHeight() * 1.0f - ImGui::GetFontSize() * 10.0f));
		ImGui::Columns(2);

		try {
			for (auto& p : fs::directory_iterator(currentPath)) {
				std::string path = p.path().filename().string();
				if (p.is_directory() && path.find('.') == std::string::npos) {
					ImGui::Button(path.c_str());
					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
						std::cout << p.path() << std::endl;
						currentPath = std::filesystem::directory_entry(p.path());
						pathName = currentPath.path().string();
					}
				}
				else {
					ImGui::TextColored((ImVec4)ImColor(100, 100, 100), path.c_str());
				}
				ImGui::NextColumn();
			}
		}
		catch (const std::exception&) {
			try {
				currentPath = std::filesystem::directory_entry(currentPath.path().parent_path());
			}
			catch (const std::exception&) {
				currentPath = std::filesystem::directory_entry("D:\\");
			}
			pathName = currentPath.path().string();
		}

		ImGui::EndChild();

		ImGui::BeginChild("##BottomRegion");

		n = projectName.length() + 64;
		char* buf2 = new char[n];
		strcpy(buf2, projectName.c_str());
		ImGui::InputText("Project Name###createProject", buf2, n);

		projectName = std::string(buf2);
		delete[] buf2;

		if (ImGui::Button("Create Project")) {
			illusioneditor::project::tools::CreateProject(pathName, projectName);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load Project")) {
			std::string error;
			bool success = illusioneditor::project::tools::LoadProject(pathName, &error);

			INTERNAL_INFO("SUCCESS ", success, " -> ", error);
		}
		ImGui::EndChild();

		ImGui::End();
	}

}