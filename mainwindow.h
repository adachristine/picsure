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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>

class ImageView;
class QMimeDatabase;
class QDir;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static bool pathIsImage(const QString &path);

public slots:
    void imageChanged(const QImage *i);
    void zoomChanged(qreal z);

    const QString &currentPath(void);
    void setCurrentPath(const QString &path);

    void zoomIn();
    void zoomOut();
    void zoomFit();
    void zoomFull();

    void next();
    void prev();

    void open();

protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private:
    static const QStringList m_accepted_types;
    static const QMimeDatabase *m_mimedatabase;
    static const qreal m_zoomadj;

    ImageView *m_view;
    QDir m_currentdir;
    QString m_currentpath;
    QToolBar *m_toolbar;

    QAction *m_zoomin_act;
    QAction *m_zoomout_act;

    QAction *m_zoomfit_act;
    QAction *m_zoomfull_act;

    QAction *m_next_act;
    QAction *m_prev_act;

    QAction *m_open_act;
};



#endif // MAINWINDOW_H
