#pragma once

#include <windows.h>
#include <GL/glu.h>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>

#include <QBasicTimer>
#include <QTime>
#include <QOpenGLTexture>

#include "utilities/mathtypes.h"
#include "utilities/utilities.h"
#include "engine/resources/models/modelsloader.h"
#include "engine/resources/textures/texturesloader.h"

#include "common/commonheaders.h"
#include "engine/components/entity.h"
#include "engine/components/transform.h"
#include "engine/core/time.h"

class SceneEditorWidget : public QOpenGLWidget, public QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit SceneEditorWidget(QWidget *parent = nullptr);
    ~SceneEditorWidget() override;
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void timerEvent(QTimerEvent *e) override;

    //Initializers
    void initShaders();

private:
    QBasicTimer timer;
    QTime lastUpdate;
    int elapsed{0};

    QOpenGLShaderProgram program;
    QOpenGLTexture *texture;

    QMatrix4x4 projection;

    illusion::utl::vector<illusion::math::Vector3> vertices_off;
    illusion::utl::vector<u32> indices_off;

signals:

public slots:
};
