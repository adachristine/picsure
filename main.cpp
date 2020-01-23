//picsure: a simple image viewer
//Copyright (C) 2020 Ada Putnam

//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "mainwindow.h"
#include <QApplication>
#include <QUrl>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    a.setApplicationDisplayName("picsure");
    a.setApplicationName("picsure");
    a.setApplicationVersion("0.1.0");
    a.setOrganizationName("adasoft");

    if (argc == 2)
    {
        // See if the argument passed is a local file
        QString path(argv[1]);
        // TODO: still constructin an image to check validity
        auto i = new QImage(path);

        if (i->format() != QImage::Format_Invalid)
        {
            w.setCurrentPath(path);
        }

        delete i;
    }

    w.show();

    return a.exec();
}
