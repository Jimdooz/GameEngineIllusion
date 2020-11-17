#ifndef TRANSFORMTEMPLATE_H
#define TRANSFORMTEMPLATE_H

#include <QWidget>
#include "componenttemplate.h"
#include "atom/atomvector3.h"

#include "engine/components/transform.h"

class TransformTemplate : public ComponentTemplate
{
    Q_OBJECT
public:
    explicit TransformTemplate(QWidget *parent = nullptr);
    void apply_transform_entity(illusion::transform::Component component);

protected:
    AtomVector3 *position;
    AtomVector3 *rotation;
    AtomVector3 *scale;

    illusion::math::Vector3 *position_e;
    illusion::math::Vector3 *rotation_e;
    illusion::math::Vector3 *scale_e;

    illusion::math::Vector3 *position_c;
    illusion::math::Quaternion *rotation_c;
    illusion::math::Vector3 *scale_c;

signals:

public slots:
    void position_change(illusion::math::Vector3*);
    void rotation_change(illusion::math::Vector3*);
    void scale_change(illusion::math::Vector3*);
};

#endif // TRANSFORMTEMPLATE_H
