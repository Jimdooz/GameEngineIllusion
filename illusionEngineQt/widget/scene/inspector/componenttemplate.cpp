#include "componenttemplate.h"


ComponentTemplate::ComponentTemplate(QString title_header, QWidget *parent) : QWidget(parent) {

    main_box = new QVBoxLayout(this);
    main_box->setMargin(0);
    main_box->setAlignment(Qt::AlignTop);

    QWidget *top_widget_component = new QWidget();
    top_widget_component->setStyleSheet("padding: 10px;margin:0px;background: #3e3e3e;");
    top_component = new QHBoxLayout(top_widget_component);

    QWidget *bot_widget_component = new QWidget();
    bot_widget_component->setStyleSheet("padding: 0px;margin:0px;");
    bot_component = new QVBoxLayout(bot_widget_component);

    QLabel *title = new QLabel(title_header);
    title->setStyleSheet("color:white;font-weight: bold;padding:0px;margin:0px;");

    top_component->addWidget(title);

    QWidget *horizontalLineWidget = new QWidget;
    horizontalLineWidget->setFixedHeight(1);
    horizontalLineWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    horizontalLineWidget->setStyleSheet(QString("background-color: #1a1a1a;margin:0px;"));

    main_box->addWidget(top_widget_component);
    main_box->addWidget(bot_widget_component);
    main_box->addWidget(horizontalLineWidget);
}
