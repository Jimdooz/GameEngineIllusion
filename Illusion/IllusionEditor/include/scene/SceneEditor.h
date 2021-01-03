#pragma once

#include "ecs/Ecs.h"
#include "ecs/Component.h"
#include "ecs/System.h"
#include "ecs/Scene.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//TEMP RENDERING
#include "Shader.h"

#include "core/Time.h"

namespace illusioneditor::scene::editor {

	using namespace illusion;

	namespace {
		ecs::Scene *scene;
		Shader *arrowShader;

		f32 nearItem = -1;

		bool translateSelection(false);

		Vec3 savedItemPosition;
		Vec3 originPositionMove;
		core::physics::primitives::Ray directionSaved;
		core::physics::primitives::Ray directionISaved;

		u8 lastTick = 0;
	}

	void Initialize() {
		arrowShader = new Shader("..\\..\\..\\GameEngineIllusion\\GameProjects\\Optimulus\\Assets\\Shader\\vertexShader.glsl",
							"..\\..\\..\\GameEngineIllusion\\GameProjects\\Optimulus\\Assets\\Shader\\fragmentArrow.glsl");
	}

	inline bool CanChangeItem() {
		return !translateSelection;
	}

	void ClickedNearTest(ecs::Scene &scene, ecs::entity_id id, Vec3 rayWord) {
		if (lastTick != Time::tick) {
			nearItem = -1;
			lastTick = Time::tick;

			if (Input::isMouseDown(0) && !ImGui::IsAnyItemHovered() && !ImGui::IsAnyWindowHovered() && CanChangeItem()) {
				views::GameHiearchy::selected = (ecs::entity_id)ecs::id::invalid_id;
			}
		}

		ecs::core::Transform& transform = *scene.GetComponent<ecs::core::Transform>();
		ecs::core::Camera& camera = *scene.GetComponent<ecs::core::Camera>();

		ecs::component_id index = transform.getIndex(id);

		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(transform.modelTransform[index], scale, rotation, translation, skew, perspective);

		Vec3 size = Vec3(0.5, 0.5, 0.5);

		core::physics::primitives::OBB obb(translation, scale * size, glm::inverse(glm::toMat4(rotation)));
		core::physics::primitives::Ray ray(Vec3(transform.position[transform.getIndex(camera.ToEntity[0])]), rayWord);

		f32 cast = core::physics::collisions::Raycast(obb, ray);

		if (Input::isMouseDown(0) && cast >= 0 && (cast < nearItem || nearItem == -1) && CanChangeItem()) {
			views::GameHiearchy::selected = id;
			nearItem = cast;
		}
	}

	bool CastBoxAxis(core::physics::primitives::OBB& box, const Vec3& axis, core::physics::primitives::Ray& rayWord) {
		core::physics::primitives::Ray ray(box.position, axis);
		core::physics::primitives::Ray rayI(box.position, -axis);
		f32 cast = core::physics::collisions::Raycast(box, rayWord);

		core::physics::primitives::Line intersectionRay, intersectionRay2;
		bool result = core::physics::collisions::RayRayIntersect(ray, rayWord, &intersectionRay);
		bool result2 = core::physics::collisions::RayRayIntersect(rayI, rayWord, &intersectionRay2);

		Vec3 pointF = core::physics::collisions::ClosestPoint(ray, intersectionRay.start);
		Vec3 pointB = core::physics::collisions::ClosestPoint(rayI, intersectionRay2.start);

		Vec3 outputPoint = glm::distance(pointF, ray.origin) > glm::distance(pointB, ray.origin) ? pointF : pointB;
		
		if (cast >= 0) {
			directionSaved = ray;
			directionISaved = rayI;
			originPositionMove = outputPoint;
		}

		return cast >= 0;
	}

	void EditorTests(ecs::Scene& scene, Vec3& rayWord) {
		if (!Input::isMouse(0)) {
			translateSelection = false;
		}

		if (ecs::id::IsValid(views::GameHiearchy::selected)) {
			ecs::core::Transform& transform = *scene.GetComponent<ecs::core::Transform>();
			ecs::core::Camera& camera = *scene.GetComponent<ecs::core::Camera>();

			ecs::component_id idTransform = transform.getIndex(views::GameHiearchy::selected);
			ecs::component_id indexCamera = transform.getIndex(camera.ToEntity[0]);

			glm::vec3 scale;
			glm::quat rotation;
			glm::vec3 translation;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(transform.ComputeModel(idTransform), scale, rotation, translation, skew, perspective);

			Vec3 CameraPosition = transform.position[indexCamera];

			core::physics::primitives::Ray ray(Vec3(transform.position[indexCamera]), rayWord);

			if (translateSelection) {
				core::physics::primitives::Line intersectionRay, intersectionRay2;
				bool result = core::physics::collisions::RayRayIntersect(directionSaved, ray, &intersectionRay);
				bool result2 = core::physics::collisions::RayRayIntersect(directionISaved, ray, &intersectionRay2);

				Vec3 pointF = core::physics::collisions::ClosestPoint(directionSaved, intersectionRay.start);
				Vec3 pointB = core::physics::collisions::ClosestPoint(directionISaved, intersectionRay2.start);

				Vec3 point = glm::distance(pointF, directionSaved.origin) > glm::distance(pointB, directionSaved.origin) ? pointF : pointB;
				transform.position[idTransform] = savedItemPosition + (point - originPositionMove);
				if (scene.entities.IsAlive(transform.parent[idTransform])) {
					Mat4x4 inverseTransform = glm::inverse(transform.ComputeModel(transform.getIndex((ecs::entity_id)transform.parent[idTransform])));
					transform.position[idTransform] = Vec3(inverseTransform * Vec4((savedItemPosition + (point - originPositionMove)), 1.0));
				}
				transform.ComputeModel(idTransform);
			}
			else {
				f32 distanceObj = glm::distance(translation, CameraPosition);
				f32 factor = distanceObj * 0.1;

				core::physics::primitives::OBB obbX(translation + (Vec3(0.7, 0, 0) * factor), Vec3(0.5, 0.5, 0.5) * Vec3(1, 0.3, 0.3) * factor);
				core::physics::primitives::OBB obbY(translation + (Vec3(0, 0.7, 0) * factor), Vec3(0.5, 0.5, 0.5) * Vec3(0.3, 1, 0.3) * factor);
				core::physics::primitives::OBB obbZ(translation + (Vec3(0, 0, 0.7) * factor), Vec3(0.5, 0.5, 0.5) * Vec3(0.3, 0.3, 1) * factor);

				if (Input::isMouseDown(0)) {
					bool castX = CastBoxAxis(obbX, Vec3(1, 0, 0), ray);
					bool castY = CastBoxAxis(obbY, Vec3(0, 1, 0), ray);
					bool castZ = CastBoxAxis(obbZ, Vec3(0, 0, 1), ray);
					if (castX || castY || castZ) {
						translateSelection = true;
						savedItemPosition = translation;
					}
				}
			}
		}
	}

	void DrawArrow(Vec3& origin, const Vec3& direction, f32 scale, const Vec4& color) {
		Mat4x4 model = glm::translate(origin + direction * 0.7 * scale) * glm::orientation(direction, Vec3(0, 1, 0)) * glm::scale(Vec3(0.05, 1, 0.05) * scale);
		arrowShader->setVec4("color", color);
		arrowShader->setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	void DrawArrowTranslate(ecs::Scene& scene, Mat4x4& projection, Mat4x4& view) {
		if (ecs::id::IsValid(views::GameHiearchy::selected)) {
			ecs::core::Transform& transform = *scene.GetComponent<ecs::core::Transform>();
			ecs::core::Camera& camera = *scene.GetComponent<ecs::core::Camera>();

			glDisable(GL_DEPTH_TEST);
			arrowShader->use();
			ecs::component_id idTransform = transform.getIndex((ecs::entity_id)ecs::id::Index(views::GameHiearchy::selected));

			glm::vec3 scale;
			glm::quat rotation;
			glm::vec3 translation;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(transform.modelTransform[idTransform], scale, rotation, translation, skew, perspective);

			Vec3 CameraPosition = transform.position[transform.getIndex(camera.ToEntity[0])];

			arrowShader->setMat4("projection", projection);
			arrowShader->setMat4("view", view);

			f32 factor = glm::distance(translation, CameraPosition) * 0.1;

			DrawArrow(translation, Vec3(1, 0, 0), factor, Vec4(242.0 / 255.0, 80.0 / 255.0, 98.0 / 255.0, 1));
			DrawArrow(translation, Vec3(0, 1, 0), factor, Vec4(78.0 / 255.0, 144 / 255.0, 240 / 255.0, 1));
			DrawArrow(translation, Vec3(0, 0, 1), factor, Vec4(139 / 250.0, 210 / 250.0, 68 / 250.0, 1));
		}
	}
}