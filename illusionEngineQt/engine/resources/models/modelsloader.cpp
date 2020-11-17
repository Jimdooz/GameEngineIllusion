#include "modelsloader.h"
#include <QFile>
#include <iostream>

namespace illusion::resources::models {

    void loadOFF(QString path, utl::vector<math::Vector3> &vertices, utl::vector<u32> &indices){
        QFile qFile(path);
        if (!qFile.open(QIODevice::ReadOnly | QIODevice::Text))
               assert(false);

        QString line;
        for(;;){
            line = qFile.readLine();
            line = line.chopped(1);
            char testComment = line[0].toLatin1();
            if(testComment != '#') break;
        }

        assert(line == "OFF");

        line = qFile.readLine();
        line = line.chopped(1);
        QStringList fields = line.split(" ");

        int vsize = fields[0].toInt();
        int isize = fields[1].toInt();

        vertices.resize(vsize);
        indices.resize(isize * 3);

        for (int i = 0; i < vsize; i++) {
            line = qFile.readLine();
            line = line.chopped(1);
            fields = line.split(" ");
            float x, y, z;
            x = fields[0].toFloat();
            y = fields[1].toFloat();
            z = fields[2].toFloat();
            math::Vector3 v = math::Vector3(x, y, z);
            vertices.at(i) = v;
        }

        for (int i = 0; i < isize; i++) {
            line = qFile.readLine();
            line = line.chopped(1);
            fields = line.split(" ");
            indices.at(i * 3) = fields[1].toUInt();
            indices.at(i * 3 + 1) = fields[2].toUInt();
            indices.at(i * 3 + 2) = fields[3].toUInt();
        }

        qFile.close();
    }

}
