#include "graphobjectelement.h"

#include <QMenu>

GraphObjectElement::GraphObjectElement(game_entity::Entity entity) : QTreeWidgetItem() {
    this->entity = entity;
}

GraphObjectElement::GraphObjectElement(game_entity::Entity entity, QTreeWidget* parent) : QTreeWidgetItem(parent) {
    this->entity = entity;
}

void GraphObjectElement::destroy(){
    for(int i = 0; i < this->childCount(); i++){
        static_cast<GraphObjectElement*>(this->child(i))->destroy();
    }
    delete(this);
}
