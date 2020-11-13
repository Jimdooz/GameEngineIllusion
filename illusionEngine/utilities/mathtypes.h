#pragma once
#include "common/commonheaders.h"

#define USE_QT_MATHS 1

namespace illusion::math {
    constexpr float PI = 3.14159265359f;
    constexpr float EPSILON = 1e-5f;
}

#if USE_QT_MATHS
#include <QtMath>
#include <QVector2D>
#include <QVector3D>
#include <QQuaternion>

#include <QMatrix4x4>

namespace illusion::math {
    using Vector2 = QVector2D;
    using Vector3 = QVector3D;
    using Quaternion = QQuaternion;

    using Matrix4x4 = QMatrix4x4;
}

#endif
