#pragma once

#include "ecs/System.h"
#include "ecs/Scene.h"
#include "ecs/CoreComponents/Transform.h"
#include "core/animation/CoreComponents/Animator.h"

namespace illusion::core::animation {

	using namespace illusion::ecs;

	struct AnimatorSystem : public System {
		SYSTEM_NAME("Animator System");
		SYSTEM_REGISTER(AnimatorSystem);

		ecs::core::Transform* transforms;
		Animator* animators;

		/* la fonction Update */
		virtual void LateUpdate() override {
			u32 size = static_cast<u32>(ToEntity.size());
			for (currIndex = 0; currIndex < size; currIndex++) {
		
				// @Todo : Compute id relative
				AnimatorElement &anim = animator();
				if (!anim.HasAnimations()) continue;
				Animation &animation = anim.animations[anim.currentAnimation];

				anim.UpdateTime(Time::deltaTime); //Update time of the animation

				if (!animation.idsComputed) {
					for (size_t i = 0, size = animation.channels.size(); i < size; i++) {
						animation.channels[i].id = transforms->FindByName(id(), animation.channels[i].relativePath);
					}
					animation.idsComputed=true;
				}

				TimeAnimation time{ anim.currentTime, animation.duration };
				for (size_t i = 0, size = animation.channels.size(); i < size; i++) {
					ecs::component_id index = transforms->getIndex(animation.channels[i].id);
					animation.channels[i].ComputeTransformations(time);
					transforms->position[index] = animation.channels[i].computedPosition;
					transforms->rotation[index] = animation.channels[i].computedRotation;
					transforms->scale[index] = animation.channels[i].computedScale;
				}
				// @Todo : Compute new model for transform
				//INTERNAL_INFO("LATE ANIMATION FOR ", id(), model());
			}
		}

		/* Definition des variables utiles */
		SYSTEM_USE_DATA(animator, animators, animator, AnimatorElement);
		SYSTEM_USE_DATA(model, transforms, modelTransform, Mat4x4);

		/* Initialisation relative a la scene parente */
		virtual void Initialize(ecs::Scene& scene) override {
			transforms = scene.GetComponent<ecs::core::Transform>();
			animators = scene.GetComponent<Animator>();
			SetDependencies(animators);
		}
	};

}
