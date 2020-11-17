#include "inspectorwidget.h"
#include "illusionengine.h"
#include "engine/components/script.h"

#include <QLineEdit>
#include <QDoubleValidator>
#include <QCheckBox>

InspectorWidget::InspectorWidget(QWidget *parent) : QWidget(parent)
{
    this->setAutoFillBackground(true);
    this->setPalette(QPalette(QPalette::Background, IllusionEngine::backgroundColor));

    component_search = new ComponentSearch(this);
    component_search->hide();
    connect(component_search, SIGNAL(component_is_selected(QString)), this, SLOT(component_selected(QString)));

    main_box = new QVBoxLayout(this);
    main_box->setMargin(0);
    main_box->setSpacing(0);
    main_box->setAlignment(Qt::AlignTop);

    QHBoxLayout *top_vertical = new QHBoxLayout(this);
    top_vertical->setMargin(5);

    QLineEdit *test_line = new QLineEdit();
    //test_line->setValidator( new QDoubleValidator(0, 100, 2, this) );

    QCheckBox *active_game_entity = new QCheckBox();

    top_vertical->addWidget(active_game_entity);
    top_vertical->addWidget(test_line);

    //Add components box
    box_components = new QVBoxLayout(this);

    //Add element
    transform = new TransformTemplate();

    //Button add component
    add_component = new QPushButton("Add Component");
    add_component->setStyleSheet("background: #585858;border: 1px solid #1a1a1a; border-radius: 3px;color:white;padding:5px; margin: 10px 20px;margin-bottom: 0px;");
    connect(add_component, SIGNAL(clicked()), this, SLOT(add_component_clicked()));

    component_search->move(add_component->pos());

    QWidget *horizontalLineWidget = new QWidget;
    horizontalLineWidget->setFixedHeight(1);
    horizontalLineWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    horizontalLineWidget->setStyleSheet(QString("background-color: #1a1a1a;"));

    main_box->addLayout(top_vertical);
    main_box->addWidget(horizontalLineWidget);
    main_box->addWidget(transform);
    main_box->addLayout(box_components);
    main_box->addSpacing(10);
    main_box->addWidget(add_component);

    transform->hide();
}

void InspectorWidget::component_selected(QString c){
    component_search->hide();

    if(entity.is_valid()){
        using namespace illusion::script;

        std::cout << "Component selected : " << c.toStdString() << std::endl;
        std::cout << "Search script creator : " << detail::get_script_creator(detail::string_hash()(c.toStdString())) << std::endl;

        detail::script_creator creator = detail::get_script_creator(detail::string_hash()(c.toStdString()));
        if(creator != nullptr){
            script::Component new_component = script::create({ creator }, entity);
            entity.addComponent(new_component);
        }
        refresh_components_visual();
    }
}

void InspectorWidget::refresh_components_visual(){
    QLayoutItem* item;
    while ( ( item = box_components->layout()->takeAt(0) ) != nullptr ) {
        delete item->widget();
        delete item;
    }

    if(illusion::game_entity::is_alive(entity.get_id())){
        utl::vector<illusion::script::Component> scripts = entity.script();
        std::cout << "SCRIPTS TROUVE " << scripts.size() << std::endl;
        for(u32 i = 0; i < scripts.size(); i++){
            ComponentTemplate *test = new ComponentTemplate( QString::fromStdString(illusion::script::detail::get_script_name(scripts.at(i))) );
            box_components->addWidget(test);
        }
    }
}

void InspectorWidget::inspect_game_entity(game_entity::Entity entity){
    this->entity = entity;

    if(entity.is_valid()){
        transform->show();
        transform->apply_transform_entity(entity.transform());
        refresh_components_visual();
    }else{
        transform->hide();
    }
}

void InspectorWidget::add_component_clicked(){
    component_search->show();
    component_search->reset_component_search();
    component_search->move(mapToGlobal(add_component->geometry().center() + QPoint(-115, 20) ));
}
