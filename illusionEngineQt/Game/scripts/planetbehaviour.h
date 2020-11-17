#ifndef PLANETBEHAVIOUR_H
#define PLANETBEHAVIOUR_H

#include "engine/api/gameentity.h"

class PlanetBehaviour : public illusion::script::EntityScript
{
public:
    constexpr explicit PlanetBehaviour(illusion::game_entity::Entity entity) : illusion::script::EntityScript(entity){}

    void Update(Time time) override;

protected:
};

#endif // PLANETBEHAVIOUR_H
