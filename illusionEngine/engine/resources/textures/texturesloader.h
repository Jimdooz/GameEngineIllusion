#pragma once

#include <QOpenGLTexture>


namespace illusion::resources::textures {
    using Texture = QOpenGLTexture;

    Texture* loadPNG(QString path);
}
