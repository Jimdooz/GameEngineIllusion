#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>

#include "atom/atomelement.h"
#include "utilities/utilities.h"

#include "engine/api/gameentity.h"

class ComponentSearch : public QWidget
{
Q_OBJECT
public:
        explicit ComponentSearch(QWidget *parent = nullptr);
        void reset_component_search();
protected:
    QLabel *title;
    QBoxLayout *main_box;

signals:
    void component_is_selected(QString selected);

public slots:
    void component_selected();
};
