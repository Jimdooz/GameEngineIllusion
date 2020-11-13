#pragma once

#include <QLabel>
#include <QWidget>
#include <Qt>
class AtomLabel : public QLabel {

   Q_OBJECT
public:

   explicit AtomLabel(QString text, QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags()) : QLabel(text, parent, f) {}
signals:

   void clicked(QMouseEvent* event);
protected:

   void mousePressEvent(QMouseEvent* event){
       emit clicked(event);
   }
};
