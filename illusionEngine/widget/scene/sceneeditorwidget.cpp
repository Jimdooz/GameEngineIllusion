#include <QDebug>

#include <iostream>

#include "sceneeditorwidget.h"
#include "illusionengine.h"

using namespace illusion;
using namespace illusion::math;

SceneEditorWidget::SceneEditorWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    this->setAutoFillBackground(true);
    this->setPalette(QPalette(QPalette::Background, QColor("#64a7d5")));

    lastUpdate = QTime::currentTime();
}

SceneEditorWidget::~SceneEditorWidget(){
    makeCurrent();
    //DELETE RESOURCES
    doneCurrent();
}

void SceneEditorWidget::initializeGL(){
    initializeOpenGLFunctions();
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glClearColor(0, 0, 0, 1); // Default background

    initShaders();
    //initTextures();

    texture = illusion::resources::textures::loadPNG(":/gameResources/textures/rock.png");
    texture->setMinificationFilter(QOpenGLTexture::Linear);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture->setWrapMode(QOpenGLTexture::Repeat);

    illusion::resources::models::loadOFF(":/gameResources/models/sphere.off", vertices_off, indices_off);

    glEnable(GL_DEPTH_TEST); // Enable depth buffer
    //glEnable(GL_CULL_FACE); // Enable back face culling

    timer.start(10, this);
}

void SceneEditorWidget::initShaders() {
    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();
}

void SceneEditorWidget::resizeGL(int w, int h){
    // Calculate aspect ratio
    f32 aspect = f32(w) / f32(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const f32 zNear = 0.1f, zFar = 170.0f, fov = 45.0f;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}

void SceneEditorWidget::paintGL(){
    glClearColor(0.3f, 0.3f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

//    float vertices[] = {
//        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
//         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
//         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
//        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

//        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
//         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
//         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
//        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
//        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

//        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

//         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

//        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
//         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
//         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
//        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

//        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
//         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
//        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
//    };
//    unsigned int indices[] = {  // note that we start from 0!
//        0, 1, 3,  // first Triangle
//        1, 2, 3   // second Triangle
//    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_off.size() * 3, &vertices_off[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * indices_off.size(), &indices_off[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)) );
    //glEnableVertexAttribArray(1);


    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    QMatrix4x4 view;
    view.setToIdentity();
    //view.translate(0,5.0f,-5.0f);
    view.lookAt(Vector3(0.0f,0.0f,10.0f), Vector3(0.0, 0.0, 0.0), Vector3(0,1,0) );

    texture->bind(0);                           // Bind to position 0
    program.setUniformValue("ourTexture", 0);  // Set uniform from position 0

    program.setUniformValue("view", view);
    program.setUniformValue("projection", projection);

    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

    u64 nb_entity = game_entity::get_active_slots();
    for(u32 i = 0; i < nb_entity; i++){
        //Convert to an id
        game_entity::entity_id id = static_cast<game_entity::entity_id>(i);
        id = game_entity::get_index_id(id);

        if(game_entity::is_alive(id)){
            game_entity::Entity e = game_entity::get_entity(id);

            Matrix4x4 model;
            model.setToIdentity();

            e.transform().compute_model(model);

            //f32 angle = 20.0f * i;
            //model.rotate(angle, Vector3(1.0f, 0.3f, 0.5f));
            program.setUniformValue("model", model);

            //glDrawArrays(GL_TRIANGLES, 0, 36);
            glDrawElements(GL_TRIANGLES, indices_off.size(), GL_UNSIGNED_INT, 0);
        }
    }

}

void SceneEditorWidget::timerEvent(QTimerEvent *)
{
    QTime currentTime = QTime::currentTime();
    int delta = lastUpdate.msecsTo(currentTime);
    lastUpdate = currentTime;
    elapsed += delta;

    //static_cast<double>(delta) / 1000.0
    Time time{
        static_cast<double>(delta) / 1000.0,
        static_cast<double>(elapsed) / 1000.0
    };

    const utl::vector<script::detail::script_ptr>& scripts = script::detail::all_scripts_game_entities();
    for(u32 i = 0; i < scripts.size(); i++){
        scripts.at(i)->Update(time);
    }

    update();
}
