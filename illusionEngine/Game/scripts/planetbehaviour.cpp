#include "planetbehaviour.h"
#include "common/primitivetypes.h"

using namespace illusion::math;

REGISTER_SCRIPT(PlanetBehaviour);

void PlanetBehaviour::Update(Time time){
    Quaternion new_direction = Quaternion::fromEulerAngles(0, fmod(time.time * 100.0, 360), 0);
    transform().rotation().setScalar(new_direction.scalar());
    transform().rotation().setX(new_direction.x());
    transform().rotation().setY(new_direction.y());
    transform().rotation().setZ(new_direction.z());
}
