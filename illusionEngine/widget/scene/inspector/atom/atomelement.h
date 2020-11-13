#ifndef ATOMELEMENT_H
#define ATOMELEMENT_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>

class AtomElement : public QWidget
{
    Q_OBJECT

public:
    AtomElement(QString title, QWidget *parent = nullptr);

protected:
    QString title;
    QHBoxLayout *container;
signals:

public slots:
};

#endif // ATOMELEMENT_H
