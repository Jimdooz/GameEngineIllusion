#pragma once

#include "common/CommonHeaders.h"

#include <string>

#include <bitset>
#include <chrono>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace illusioneditor::views::GameStats {

	using namespace illusion;

	struct StatElement {
		util::Array<f32> datas;
		bool open = false;

		std::string parent;
		util::Array<std::string> childs;

		std::chrono::steady_clock::time_point chronoStart;
	};

	namespace {
		util::Map<std::string, StatElement> stats;
		const u32 maxStatDatas = 64;
	}

	void Show() {
		ImGui::Begin("Stats###EngineStats");

		float total = 0.0;
		float datasGet[maxStatDatas];
		std::fill(datasGet, datasGet + maxStatDatas, 1);

		for (auto const& [key, val] : stats) {
			if (val.parent == "") {
				double total = 0;
				for (u32 i = 0; i < val.datas.size(); i++) {
					datasGet[i] = val.datas[i];
					total += val.datas[i];
				}
				total /= 1000000;
				total = total / val.datas.size();
				ImGui::Text(key.c_str());
				ImGui::SameLine();
				ImGui::TextColored((ImVec4)ImColor::ImColor(100, 100, 100), std::string(std::to_string(total) + "s").c_str());
				ImGui::SameLine();
				ImGui::TextColored((ImVec4)ImColor::ImColor(100, 200, 100), std::string(std::to_string((int)round(1 / total)) + " fps").c_str());
				ImGui::PlotLines("", datasGet, val.datas.size());

				ImGui::BeginChild("##ProjectRegion");
				ImGui::Columns(3);
				for (u32 i = 0; i < val.childs.size(); i++) {
					StatElement& val2 = stats[val.childs[i]];
					double total2 = 0;
					for (u32 i = 0; i < val2.datas.size(); i++) {
						datasGet[i] = val2.datas[i];
						total2 += val2.datas[i];
					}
					total2 /= 1000000;
					total2 = total2 / val2.datas.size();
					ImGui::Text(std::string(val.childs[i] + "\t").c_str());
					ImGui::NextColumn();
					ImGui::TextColored((ImVec4)ImColor::ImColor(100, 100, 100), std::string(std::to_string(total2) + "s").c_str());
					ImGui::NextColumn();
					ImGui::TextColored((ImVec4)ImColor::ImColor(100, 150, 100), std::string(std::to_string((total2 / total) * 100.0) + " %%%").c_str());
					ImGui::NextColumn();
				}
				ImGui::EndChild();
			}

		}

		ImGui::End();
	}

	void CreateStatElement(std::string name, std::string parent = "") {
		if (stats.find(name) == stats.end()) {
			stats[name] = StatElement();
			stats[name].parent = parent;

			if (parent == "") return;
			CreateStatElement(parent);
			stats[parent].childs.push_back(name);
		}
	}

	void FillDatas(std::string name, double data, std::string parent = "") {
		CreateStatElement(name, parent);

		stats[name].datas.push_back(data);
		if (stats[name].datas.size() >= maxStatDatas) stats[name].datas.erase(stats[name].datas.begin());
	}

	void StartChronoData(std::string name, std::string parent = "") {
		CreateStatElement(name, parent);
		stats[name].chronoStart = std::chrono::high_resolution_clock::now();
	}

	void EndChronoData(std::string name, std::string parent = "") {
		CreateStatElement(name, parent);
		auto chronoEnd = std::chrono::high_resolution_clock::now();

		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(chronoEnd - stats[name].chronoStart).count();
		duration = duration > 0 ? duration : 1;
		FillDatas(name, duration, parent);
	}
}