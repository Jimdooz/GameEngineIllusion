#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QVBoxLayout>

#include <QStandardItem>

#include "graph/graphobjectelement.h"
#include "utilities/utilities.h"

#include "common/commonheaders.h"
#include "engine/components/entity.h"
#include "engine/components/transform.h"

class GraphSceneWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GraphSceneWidget(QWidget *parent = nullptr);

    void load_game_entities();
    void load_entity(illusion::game_entity::entity_id id);

    GraphObjectElement* add_element(game_entity::Entity entity);
    GraphObjectElement* add_element(game_entity::Entity entity, GraphObjectElement *parent);
    void remove_element(game_entity::Entity entity);


protected:
    QTreeWidget *graph_element;
    illusion::utl::map<illusion::game_entity::entity_id, GraphObjectElement*> object_items;

signals:
    void on_game_entity_selected(game_entity::Entity entity);

public slots:
    void show_context_menu(const QPoint &pos);
    void create_new_game_entity();
    void delete_game_entity();
    void selection_changed(QTreeWidgetItem *current, QTreeWidgetItem *previous);
};
