#include "atomelement.h"

AtomElement::AtomElement(QString title, QWidget *parent) : QWidget(parent)
{
    this->setStyleSheet("padding: 0px; margin: 0px;");
    this->title = title;

    this->container = new QHBoxLayout(this);
    container->setMargin(2);
    QLabel *title_label = new QLabel(this->title);
    title_label->setStyleSheet("min-width: 100px;color:#dbdbdb;");

    this->container->addWidget(title_label);
}
