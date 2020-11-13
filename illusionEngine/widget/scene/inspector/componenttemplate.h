#pragma once

#include <QWidget>
#include <QVBoxLayout>

#include "atom/atomelement.h"
#include "utilities/utilities.h"

class ComponentTemplate : public QWidget
{
Q_OBJECT
public:
        explicit ComponentTemplate(QString title_header, QWidget *parent = nullptr);
protected:
    QBoxLayout *main_box;

    QString title;
    QIcon *icon;
    bool disable;

    illusion::utl::vector<AtomElement*> elements;
    QHBoxLayout *top_component;
    QVBoxLayout *bot_component;

signals:

public slots:
};
