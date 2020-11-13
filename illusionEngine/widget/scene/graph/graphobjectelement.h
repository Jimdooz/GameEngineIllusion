#pragma once

#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "common/commonheaders.h"
#include "engine/components/entity.h"
#include "engine/components/transform.h"

using namespace illusion;

class GraphObjectElement : public QTreeWidgetItem
{
public:
    GraphObjectElement(game_entity::Entity entity);
    GraphObjectElement(game_entity::Entity entity, QTreeWidget* parent);
    const game_entity::Entity get_entity(){ return entity; }
    void destroy();

protected:
    game_entity::Entity entity;
};
