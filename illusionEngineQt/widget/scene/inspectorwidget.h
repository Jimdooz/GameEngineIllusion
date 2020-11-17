#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>

#include "graph/graphobjectelement.h"
#include "utilities/utilities.h"

#include "common/commonheaders.h"
#include "engine/components/entity.h"
#include "engine/components/transform.h"

#include "inspector/componenttemplate.h"
#include "inspector/transformtemplate.h"
#include "inspector/componentsearch.h"

class InspectorWidget : public QWidget
{
    Q_OBJECT
public:
    InspectorWidget(QWidget *parent = nullptr);

protected:
    void refresh_components_visual();

    QBoxLayout *main_box;
    game_entity::Entity entity;
    TransformTemplate *transform;
    QVBoxLayout *box_components;
    utl::vector<ComponentTemplate> components;

    QPushButton *add_component;
    ComponentSearch *component_search;

signals:

public slots:
    void inspect_game_entity(game_entity::Entity entity);
    void add_component_clicked();
    void component_selected(QString c);
};
