/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QApplication>
#include <QSurface>
#include <iostream>

#include "mainwindow.h"
#include "illusionengine.h"

#include "common/commonheaders.h"
#include "engine/components/entity.h"
#include "engine/components/transform.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setStyleSheet(R"(
        QSplitter::handle {
          background: #191919;
          padding-left: 0px;
          padding-right: -3px;
        }
        QScrollBar:horizontal {
            background: transparent;
            border: 0 solid #191919;
            margin: 0px 0px 0px 0px;
            padding: 3px;
        }
        QScrollBar::handle:horizontal {
            background: rgba(0,0,0,0.2);
            border-radius : 5px;
        }
        QScrollBar::add-line:horizontal {
            background: rgba(0,0,0,0.2);
            width: 0px;
        }
        QScrollBar::sub-line:horizontal {
            background:rgba(0,0,0,0.2);
            width: 0px;
        }
        QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
            background: none;
        }
        QScrollBar:vertical {
          background: transparent;
          border: 0 solid #191919;
          margin: 0px 0px 0px 0px;
          padding: 3px;
        }
        QScrollBar::handle:vertical {
          background: rgba(0,0,0,0.2);
          border-radius : 5px;
        }
        QScrollBar::add-line:vertical {
          background: rgba(0,0,0,0.2);
          height: 0px;
        }
        QScrollBar::sub-line:vertical {
          background:rgba(0,0,0,0.2);
          height: 0px;
        }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
          background: none;
        }
        QLineEdit{
            margin : 2px;
            border-radius: 3px;
            border: 1px solid #191919;
            background: 1px solid #2a2a2a;
            color: white;
            padding: 1px 3px;
        }
        QCheckBox {
            margin : 2px;
        }

        QCheckBox::indicator {
            border-radius: 3px;
            border: 1px solid #191919;
            background: 1px solid #2a2a2a;
            height: 12px;
            width: 12px;
            padding: 3px;
        }

        QCheckBox::indicator:checked {
            image: url(:/img/windowResources/icons/checked.svg);
        }
    )");

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    //Set Engine icon
    app.setWindowIcon(QIcon(IllusionEngine::iconPath));

    //Application Version & Name
    app.setApplicationName("Illusion");
    app.setApplicationVersion("0.0.1");

    MainWindow w;
    w.show();

    std::cout << "Start" << std::endl;

    return app.exec();
}
