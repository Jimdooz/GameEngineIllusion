QT       += core gui widgets

CONFIG += c++17

TARGET = gameengine
TEMPLATE = app

SOURCES += main.cpp \
    Game/scripts/planetbehaviour.cpp \
    engine/components/entity.cpp \
    engine/components/mesh/meshfilter.cpp \
    engine/components/mesh/meshrenderer.cpp \
    engine/components/script.cpp \
    engine/components/transform.cpp \
    engine/core/mesh.cpp \
    engine/resources/models/modelsloader.cpp \
    engine/resources/textures/texturesloader.cpp \
    illusionengine.cpp \
    mainwindow.cpp \
    widget/scene/graph/graphobjectelement.cpp \
    widget/scene/graphscenewidget.cpp \
    widget/scene/inspector/atom/atomelement.cpp \
    widget/scene/inspector/atom/atomvector3.cpp \
    widget/scene/inspector/componentsearch.cpp \
    widget/scene/inspector/componenttemplate.cpp \
    widget/scene/inspector/transformtemplate.cpp \
    widget/scene/inspectorwidget.cpp \
    widget/scene/sceneeditorwidget.cpp

SOURCES +=

HEADERS += \
    Game/scripts/planetbehaviour.h \
    common/commonheaders.h \
    common/id.h \
    common/primitivetypes.h \
    engine/api/gameentity.h \
    engine/api/mesh/meshfiltercomponent.h \
    engine/api/mesh/meshrenderercomponent.h \
    engine/api/scriptcomponent.h \
    engine/api/transformcomponent.h \
    engine/components/componentcommon.h \
    engine/components/entity.h \
    engine/components/mesh/meshfilter.h \
    engine/components/mesh/meshrenderer.h \
    engine/components/script.h \
    engine/components/transform.h \
    engine/core/mesh.h \
    engine/core/time.h \
    engine/resources/filesystem.h \
    engine/resources/models/modelsloader.h \
    engine/resources/textures/texturesloader.h \
    illusionengine.h \
    mainwindow.h \
    utilities/mathtypes.h \
    utilities/utilities.h \
    widget/scene/graph/graphobjectelement.h \
    widget/scene/graphscenewidget.h \
    widget/scene/inspector/atom/atomelement.h \
    widget/scene/inspector/atom/atomlabel.h \
    widget/scene/inspector/atom/atomvector3.h \
    widget/scene/inspector/componentsearch.h \
    widget/scene/inspector/componenttemplate.h \
    widget/scene/inspector/transformtemplate.h \
    widget/scene/inspectorwidget.h \
    widget/scene/sceneeditorwidget.h

RESOURCES += \
    models.qrc \
    shaders.qrc \
    textures.qrc \
    windowEngine.qrc

# install
target.path = $$[YOUR_PATH]
INSTALLS += target
