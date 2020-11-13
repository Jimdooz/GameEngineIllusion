#include "componentsearch.h"

ComponentSearch::ComponentSearch(QWidget *parent) : QWidget(parent) {
    this->setStyleSheet("background : #383838;border: 1px solid #232323;width: 230px;");
    setWindowFlags(Qt::Popup);

    main_box = new QVBoxLayout(this);
    main_box->setMargin(0);
    main_box->setSpacing(0);
    main_box->setAlignment(Qt::AlignTop);

    title = new QLabel("Component");
    title->setStyleSheet("background: #505050; color: #a4a4a4;padding: 5px;margin:0px;");

    main_box->addWidget(title);
}

void ComponentSearch::reset_component_search(){
    illusion::utl::vector<std::string>& script_names = illusion::script::detail::get_scripts_name();

    QLayoutItem* item;
    while ( ( item = main_box->layout()->takeAt(1) ) != nullptr ) {
        delete item->widget();
        delete item;
    }

    for(u32 i = 0; i < script_names.size(); i++){
        QPushButton *btn = new QPushButton(QString::fromStdString(script_names.at(i)));
        btn->setStyleSheet("padding: 5px;margin:0px;border:0px;text-align:left; color: #a4a4a4;border: 1px solid #232323;border-top:0px;");
        main_box->addWidget(btn);

        connect(btn, SIGNAL(clicked()), this, SLOT(component_selected()));
    }
}

void ComponentSearch::component_selected(){
    auto button = qobject_cast<QPushButton *>(sender());
    Q_ASSERT(button);

    component_is_selected(button->text());
}
