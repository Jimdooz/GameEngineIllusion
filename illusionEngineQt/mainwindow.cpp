#include "mainwindow.h"
#include "illusionengine.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Illusion Engine");
    resize(1280,720);

    //Creation of the main Widget
    _mainContainer = new QWidget(this);
    _mainContainer->setAutoFillBackground(true);
    _mainContainer->setPalette(QPalette(QPalette::Window, IllusionEngine::borderColor));

        //Add a boxContainer
        QHBoxLayout *mainVerticalBox = new QHBoxLayout(_mainContainer);
        QSplitter *mainSplitter = new QSplitter(_mainContainer);

    //Add different Widgets scene
    _graphSceneWidget = new GraphSceneWidget(this);
    _sceneEditorWidget = new SceneEditorWidget(this);
    _inspectorWidget = new InspectorWidget(this);

    //Connections
    connect(_graphSceneWidget , SIGNAL( on_game_entity_selected(game_entity::Entity) ), _inspectorWidget, SLOT( inspect_game_entity(game_entity::Entity) ));

    _inspectorWidget->setMaximumWidth(350);
    _graphSceneWidget->setMaximumWidth(350);
    _inspectorWidget->setMinimumWidth(50);
    _graphSceneWidget->setMinimumWidth(50);

    mainSplitter->addWidget(_graphSceneWidget);
    mainSplitter->addWidget(_sceneEditorWidget);
    mainSplitter->addWidget(_inspectorWidget);

    mainVerticalBox->addWidget(mainSplitter);
    mainVerticalBox->setMargin(0);

    //Apply the main widget to the central element of the window
    setCentralWidget(_mainContainer);
}

MainWindow::~MainWindow(){

}
