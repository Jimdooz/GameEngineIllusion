#ifndef ATOMVECTOR3_H
#define ATOMVECTOR3_H

#include "atomelement.h"
#include <QLineEdit>

#include "utilities/utilities.h"
#include "utilities/mathtypes.h"

class AtomVector3 : public AtomElement
{
    Q_OBJECT
public:
    explicit AtomVector3(QString name, QWidget *parent = nullptr);
    void set_active_vector(illusion::math::Vector3* new_vector);

protected:
    QLineEdit *arg1;
    QLineEdit *arg2;
    QLineEdit *arg3;

    int element_active = -1; // x = 0; y = 1; z = 2; undefined = -1
    double saved_value = 0;
    QPoint saved_position;

    illusion::math::Vector3* vector3{nullptr};

    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void vector_update(illusion::math::Vector3* vector_update);

public slots:
    void arg_change(const QString &text);

    void clicked_x(QMouseEvent *event);
    void clicked_y(QMouseEvent *event);
    void clicked_z(QMouseEvent *event);


};

#endif // ATOMVECTOR3_H
