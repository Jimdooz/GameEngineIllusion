#pragma once

#include <QMainWindow>

#include "widget/scene/graphscenewidget.h"
#include "widget/scene/sceneeditorwidget.h"
#include "widget/scene/inspectorwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);
    ~MainWindow();

protected:
    //Main Container
    QWidget *_mainContainer;

    //Scene Widget
    GraphSceneWidget *_graphSceneWidget;
    SceneEditorWidget *_sceneEditorWidget;
    InspectorWidget *_inspectorWidget;

};
