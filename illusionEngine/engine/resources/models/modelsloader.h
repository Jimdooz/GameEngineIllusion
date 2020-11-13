#pragma once

#include "utilities/mathtypes.h"
#include "utilities/utilities.h"
#include <QString>

namespace illusion::resources::models {

    void loadOFF(QString path, utl::vector<math::Vector3> &vertices, utl::vector<u32> &indices);
    void loadOBJ(QString path, utl::vector<math::Vector3> &vertices, utl::vector<u32> &indices);

}
