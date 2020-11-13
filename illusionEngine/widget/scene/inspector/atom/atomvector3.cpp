#include "atomvector3.h"
#include <iostream>
#include <QMouseEvent>
#include <QPoint>
#include <QApplication>
#include <QDoubleValidator>

#include "atomlabel.h"

AtomVector3::AtomVector3(QString name, QWidget *parent) : AtomElement(name, parent)
{
    QHBoxLayout *vector_element = new QHBoxLayout();
    vector_element->setMargin(0);

    AtomLabel *argX = new AtomLabel("X");
    argX->setStyleSheet("color:#dbdbdb;");
    AtomLabel *argY = new AtomLabel("Y");
    argY->setStyleSheet("color:#dbdbdb;");
    AtomLabel *argZ = new AtomLabel("Z");
    argZ->setStyleSheet("color:#dbdbdb;");

    QObject::connect(argX, SIGNAL(clicked(QMouseEvent*)), this, SLOT(clicked_x(QMouseEvent*)));
    QObject::connect(argY, SIGNAL(clicked(QMouseEvent*)), this, SLOT(clicked_y(QMouseEvent*)));
    QObject::connect(argZ, SIGNAL(clicked(QMouseEvent*)), this, SLOT(clicked_z(QMouseEvent*)));

    arg1 = new QLineEdit();
    arg2 = new QLineEdit();
    arg3 = new QLineEdit();

    connect(arg1, SIGNAL(textEdited(const QString &)), this, SLOT(arg_change(const QString &)));
    connect(arg2, SIGNAL(textEdited(const QString &)), this, SLOT(arg_change(const QString &)));
    connect(arg3, SIGNAL(textEdited(const QString &)), this, SLOT(arg_change(const QString &)));

    arg1->setText("0");
    arg2->setText("0");
    arg3->setText("0");

    vector_element->addWidget(argX);
    vector_element->addWidget(arg1);
    vector_element->addWidget(argY);
    vector_element->addWidget(arg2);
    vector_element->addWidget(argZ);
    vector_element->addWidget(arg3);

    container->addLayout(vector_element);
}

void AtomVector3::set_active_vector(illusion::math::Vector3* new_vector){
    this->vector3 = new_vector;

    if(this->vector3 != nullptr){
        arg1->setText( QString::number( this->vector3->x() ) );
        arg2->setText( QString::number( this->vector3->y() ) );
        arg3->setText( QString::number( this->vector3->z() ) );
    }
}

void AtomVector3::arg_change(const QString &){
    if(this->vector3 != nullptr){
        this->vector3->setX( arg1->text().toFloat() );
        this->vector3->setY( arg2->text().toFloat() );
        this->vector3->setZ( arg3->text().toFloat() );

        vector_update(this->vector3);
    }
}

void AtomVector3::mouseMoveEvent(QMouseEvent *event){
    double to_add = (QCursor::pos().x() - saved_position.x()) / 20.0;
    if(element_active == 0){
        arg1->setText( QString::number(saved_value + to_add) );
    }else if(element_active == 1){
        arg2->setText( QString::number(saved_value + to_add) );
    }else if(element_active == 2){
        arg3->setText( QString::number(saved_value + to_add) );
    }
    arg_change(nullptr);
}

void AtomVector3::mouseReleaseEvent(QMouseEvent *event){
    element_active = -1;
    QApplication::restoreOverrideCursor();
}

void AtomVector3::clicked_x(QMouseEvent *event){
    if (event->button() == Qt::LeftButton){
        saved_value = QString(arg1->text()).toDouble();
        element_active = 0;
        saved_position.setX(QCursor::pos().x());
        saved_position.setY(QCursor::pos().y());

        QApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
    }
}

void AtomVector3::clicked_y(QMouseEvent *event){
    if (event->button() == Qt::LeftButton){
        saved_value = QString(arg2->text()).toDouble();
        element_active = 1;
        saved_position.setX(QCursor::pos().x());
        saved_position.setY(QCursor::pos().y());

        QApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
    }
}

void AtomVector3::clicked_z(QMouseEvent *event){
    if (event->button() == Qt::LeftButton){
        saved_value = QString(arg3->text()).toDouble();
        element_active = 2;
        saved_position.setX(QCursor::pos().x());
        saved_position.setY(QCursor::pos().y());

        QApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
    }
}
