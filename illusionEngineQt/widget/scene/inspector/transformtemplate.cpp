#include "transformtemplate.h"


TransformTemplate::TransformTemplate(QWidget *parent) : ComponentTemplate("Transform", parent)
{

    position = new AtomVector3("Position");
    rotation = new AtomVector3("Rotation");
    scale = new AtomVector3("Scale");

    connect(position, SIGNAL(vector_update(illusion::math::Vector3*)), this, SLOT(position_change(illusion::math::Vector3*)));
    connect(rotation, SIGNAL(vector_update(illusion::math::Vector3*)), this, SLOT(rotation_change(illusion::math::Vector3*)));
    connect(scale, SIGNAL(vector_update(illusion::math::Vector3*)), this, SLOT(scale_change(illusion::math::Vector3*)));

    elements.push_back(position);
    elements.push_back(rotation);
    elements.push_back(scale);

    for(size_t i = 0; i < elements.size(); i++){
        bot_component->addWidget(elements.at(i));
    }

    position_e = new illusion::math::Vector3{ 0, 0, 0};
    rotation_e = new illusion::math::Vector3{ 0, 0, 0};
    scale_e = new illusion::math::Vector3{ 1, 1, 1};
}

void TransformTemplate::apply_transform_entity(illusion::transform::Component component){
    position_c = &component.position();
    rotation_c = &component.rotation();
    scale_c = &component.scale();

    if(position_c != nullptr && rotation_c != nullptr && scale_c != nullptr){
        position_e->setX(position_c->x());
        position_e->setY(position_c->y());
        position_e->setZ(position_c->z());

        illusion::math::Vector3 euler_angle = rotation_c->toEulerAngles();
        rotation_e->setX(euler_angle.x());
        rotation_e->setY(euler_angle.y());
        rotation_e->setZ(euler_angle.z());

        scale_e->setX(scale_c->x());
        scale_e->setY(scale_c->y());
        scale_e->setZ(scale_c->z());

        position->set_active_vector(position_e);
        rotation->set_active_vector(rotation_e);
        scale->set_active_vector(scale_e);
    }
}

void TransformTemplate::position_change(illusion::math::Vector3* position){
    if(position_c != nullptr && position != nullptr){
        position_c->setX(position->x());
        position_c->setY(position->y());
        position_c->setZ(position->z());
    }
}
void TransformTemplate::rotation_change(illusion::math::Vector3* rotation){
    if(rotation_c != nullptr && rotation != nullptr){
        illusion::math::Vector3* tmpRotation = new illusion::math::Vector3();
        tmpRotation->setX(std::fmod(rotation->x(), 360));
        tmpRotation->setY(std::fmod(rotation->y(), 360));
        tmpRotation->setZ(std::fmod(rotation->z(), 360));
        illusion::math::Quaternion q = illusion::math::Quaternion::fromEulerAngles(*tmpRotation);

        rotation_c->setScalar(q.scalar());
        rotation_c->setX(q.x());
        rotation_c->setY(q.y());
        rotation_c->setZ(q.z());
    }
}
void TransformTemplate::scale_change(illusion::math::Vector3* scale){
    if(scale_c != nullptr && scale != nullptr){
        scale_c->setX(scale->x());
        scale_c->setY(scale->y());
        scale_c->setZ(scale->z());
    }
}
