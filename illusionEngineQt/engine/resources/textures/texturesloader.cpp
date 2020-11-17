#include "texturesloader.h"

namespace illusion::resources::textures {

    Texture* loadPNG(QString path){
        return new Texture(QImage(path).mirrored());
    }

}
