#include "graphscenewidget.h"
#include "illusionengine.h"

#include <QMenu>

using namespace illusion;

GraphSceneWidget::GraphSceneWidget(QWidget *parent) : QWidget(parent)
{
    this->setAutoFillBackground(true);
    this->setPalette(QPalette(QPalette::Window, IllusionEngine::backgroundColor));
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(show_context_menu(const QPoint &)));

    QVBoxLayout *mainVerticalBox = new QVBoxLayout(this);
    mainVerticalBox->setMargin(0);

    graph_element = new QTreeWidget();
    graph_element->setHeaderLabel("Graph Scene");
    graph_element->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(graph_element, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(show_context_menu(const QPoint &)));
    connect(graph_element, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(selection_changed(QTreeWidgetItem *, QTreeWidgetItem *)));

    const char*cssTree =R"(
        QHeaderView::section {
            border:0;
            padding: 2px 20px;
            background-color: #505050;
            color: white;
        }
        QTreeView, QTreeWidget {
            show-decoration-selected: 1;
        }
        QTreeWidget {
            color:rgb(255,255,255);
            background-color:rgba(0,0,0,0);
            selection-background-color:transparent;
            border:none;
            outline: 0;
            qproperty-indentation: 14;
        }
        QTreeView::branch:has-children:!has-siblings:closed,
        QTreeView::branch:closed:has-children:has-siblings {
            border-image: none;
            image: url(:/img/windowResources/icons/arrowclose.svg);
            padding: 2px;
        }

        QTreeView::branch:open:has-children:!has-siblings,
        QTreeView::branch:open:has-children:has-siblings  {
            border-image: none;
            image: url(:/img/windowResources/icons/arrowopen.svg);
            padding: 2px;
        }
        QTreeView::item {
            height: 20px;
            border:none;
            outline:0;
            padding-left: 5px;
        }
        QTreeView::branch {
            background-color: rgba(0,0,0,0);
        }
        QTreeWidget::item:hover {
            background-color:rgba(255,255,255,10);
        }
        QTreeWidget::item:selected {
            background-color:rgba(44,93,135,1);
        }
        QTreeView::branch:!selected {
            background-color: rgba(0,0,0,0);
        }
        QTreeView::branch:hover {
            background-color:rgba(255,255,255,10);
        }
        QTreeView::branch:selected {
            background-color: rgba(44,93,135,1);
        }

        )";

    graph_element->setStyleSheet(cssTree);
    load_game_entities();

    mainVerticalBox->addWidget(graph_element);
}

void GraphSceneWidget::load_game_entities(){
    u64 nb_entity = game_entity::get_active_slots();
    for(u64 i = 0; i < nb_entity; i++){
        //Convert to an id
        game_entity::entity_id id = static_cast<game_entity::entity_id>(i);
        id = game_entity::get_index_id(id);

        if( game_entity::is_alive(id)){
            load_entity(id);
        }
    }
}

void GraphSceneWidget::load_entity(illusion::game_entity::entity_id id){
    if(object_items.count(id) <= 0){
        std::cout << "Not found element << " << id << std::endl;
        game_entity::Entity e = game_entity::get_entity(id);

        GraphObjectElement *itm;
        game_entity::entity_id parent_id = static_cast<game_entity::entity_id>(e.transform().parent().get_id());

        if(id::is_valid(e.transform().parent().get_id()) && game_entity::is_alive(parent_id)){
            game_entity::entity_id parent_id = static_cast<game_entity::entity_id>(e.transform().parent().get_id());
            load_entity(parent_id);

            GraphObjectElement *parent = object_items.at(parent_id);
            itm = add_element(e, parent);
        }else{
            itm = add_element(e);
        }

        object_items.emplace(id, itm);
    }
}

GraphObjectElement* GraphSceneWidget::add_element(game_entity::Entity entity){
    GraphObjectElement *itm = new GraphObjectElement(entity, graph_element);
    itm->setText(0, QString("GameObject ").append( QString::number(entity.get_id())) );
    itm->setIcon(0, QIcon(":/img/windowResources/icons/gameobject.svg"));

    graph_element->clearSelection();
    itm->setSelected(true);

    selection_changed(itm, itm);

    return itm;
}


GraphObjectElement* GraphSceneWidget::add_element(game_entity::Entity entity, GraphObjectElement *parent){
    GraphObjectElement *itm = new GraphObjectElement(entity);
    itm->setText(0, QString("GameObject ").append( QString::number(entity.get_id())) );
    itm->setIcon(0, QIcon(":/img/windowResources/icons/gameobject.svg"));

    parent->addChild(itm);

    graph_element->clearSelection();
    parent->setExpanded(true);
    itm->setSelected(true);
    selection_changed(itm, itm);


    return itm;
}

void GraphSceneWidget::remove_element(game_entity::Entity entity){
    if(object_items.count(entity.get_id()) > 0){
        GraphObjectElement *itm = object_items.at(entity.get_id());
        itm->destroy();
        object_items.erase(entity.get_id());
    }
}

void GraphSceneWidget::show_context_menu(const QPoint &pos) {
    QMenu contextMenu(tr("Context menu"), this);
    // Press paper
    QAction gm_action_copy("Copy", this);
    QAction gm_action_paste("Paste", this);
    // Operations
    QAction gm_action_rename("Rename", this);
    QAction gm_action_duplicate("Duplicate", this);
    QAction gm_action_delete("Delete", this);
    //Cration items
    QAction action_create_empty("Create Empty", this);

    GraphObjectElement *item = static_cast<GraphObjectElement*>(graph_element->itemAt(pos));
    if(item){
        contextMenu.addAction(&gm_action_copy);
        contextMenu.addAction(&gm_action_paste);
        contextMenu.addSeparator();
        contextMenu.addAction(&gm_action_rename);
        contextMenu.addAction(&gm_action_duplicate);
        contextMenu.addAction(&gm_action_delete);
        contextMenu.addSeparator();
        //Connections

        //Add Empty Connection to Parent
        game_entity::Entity e = item->get_entity();
        QVariant v = qVariantFromValue(static_cast<void*>( &e ));

        gm_action_delete.setData(v);
        connect(&gm_action_delete, SIGNAL(triggered()), this, SLOT(delete_game_entity()));

        action_create_empty.setData(v);
        connect(&action_create_empty, SIGNAL(triggered()), this, SLOT(create_new_game_entity()));
    }else{
        //Connections
        connect(&action_create_empty, SIGNAL(triggered()), this, SLOT(create_new_game_entity()));
    }

    contextMenu.addAction(&action_create_empty);

    //   connect(&action1, SIGNAL(triggered()), this, SLOT(removeDataPoint()));


    contextMenu.exec(mapToGlobal(pos + QPoint(0, contextMenu.height())));
}

void GraphSceneWidget::create_new_game_entity(){
    QAction *act = qobject_cast<QAction *>(sender());
    QVariant v = act->data();
    game_entity::Entity *parent = static_cast<game_entity::Entity*>(v.value<void *>());

    transform::transform_id parent_id = static_cast<transform::transform_id>(id::invalid_id);

    if(parent != nullptr){
        parent_id = parent->transform().get_id();
    }

    transform::init_info t_info{ parent_id };
    game_entity::entity_info info{ &t_info };
    game_entity::Entity entity{ game_entity::create(info) };

    load_entity(entity.get_id());
}

void GraphSceneWidget::delete_game_entity(){
    QAction *act = qobject_cast<QAction *>(sender());
    QVariant v = act->data();
    game_entity::Entity *entity = static_cast<game_entity::Entity*>(v.value<void *>());

    if(entity != nullptr){
        remove_element(*entity);
        game_entity::remove(entity->get_id());
    }
}

void GraphSceneWidget::selection_changed(QTreeWidgetItem *current, QTreeWidgetItem *previous){
    if(current != nullptr){
        game_entity::Entity entity = static_cast<GraphObjectElement*>(current)->get_entity();
        if(game_entity::is_alive(entity.get_id())){
            on_game_entity_selected(static_cast<GraphObjectElement*>(current)->get_entity());
        }
    }
}

