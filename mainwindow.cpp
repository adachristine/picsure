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
#include "imageview.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeDatabase>
#include <QMimeData>
#include <QKeyEvent>
#include <QFileInfo>
#include <QtMath>
#include <QtDebug>
#include <QToolBar>
#include <QStandardPaths>
#include <QFileDialog>
#include <QScopedPointer>

const QMimeDatabase *MainWindow::m_mimedatabase = new QMimeDatabase();
const QStringList MainWindow::m_accepted_types = {"image/png", "image/jpeg", "image/tiff"};
const qreal MainWindow::m_zoomadj = 0.25;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_view(new ImageView(this)),
      m_currentdir(QStandardPaths::displayName(QStandardPaths::PicturesLocation)),
      m_toolbar(new QToolBar(this))
{
    setCentralWidget(m_view);
    setAcceptDrops(true);
    resize(800,600);

    m_open_act = new QAction(tr("Open"), this);
    m_open_act->setShortcut(QKeySequence::Open);

    m_prev_act = new QAction(tr("Previous"), this);
    m_prev_act->setShortcut(QKeySequence(Qt::Key_Left));

    m_next_act = new QAction(tr("Next"), this);
    m_next_act->setShortcut(QKeySequence(Qt::Key_Right));

    m_zoomin_act = new QAction(tr("Zoom In"));
    m_zoomin_act->setShortcut(QKeySequence::ZoomIn);

    m_zoomout_act = new QAction(tr("Zoom Out"));
    m_zoomout_act->setShortcut(QKeySequence::ZoomOut);

    m_zoomfit_act = new QAction(tr("Fit View"));
    m_zoomfit_act->setShortcut(QKeySequence(Qt::Key_Backspace));

    m_zoomfull_act = new QAction(tr("Full Size"));
    m_zoomfull_act->setShortcut(QKeySequence(Qt::Key_Enter));

    auto zoom_hint_group = new QActionGroup(this);
    zoom_hint_group->setExclusionPolicy(QActionGroup::ExclusionPolicy::Exclusive);

    // TODO: full screen view
    auto left_expand = new QWidget();
    auto right_expand = new QWidget();

    left_expand->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    right_expand->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_toolbar->addWidget(left_expand);
    m_toolbar->addAction(m_open_act);
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_prev_act);
    m_toolbar->addAction(m_next_act);
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_zoomfit_act);
    m_toolbar->addAction(m_zoomout_act);
    m_toolbar->addAction(m_zoomin_act);
    m_toolbar->addAction(m_zoomfull_act);
    m_toolbar->addWidget(right_expand);

    m_toolbar->setAllowedAreas(Qt::BottomToolBarArea);
    m_toolbar->setFloatable(false);
    m_toolbar->setMovable(false);

    addToolBar(Qt::BottomToolBarArea, m_toolbar);

    connect(m_view, &ImageView::imageChanged, this, &MainWindow::imageChanged);
    connect(m_view, &ImageView::zoomChanged, this, &MainWindow::zoomChanged);

    connect(m_open_act, &QAction::triggered, this, &MainWindow::open);
    connect(m_prev_act, &QAction::triggered, this, &MainWindow::prev);
    connect(m_next_act, &QAction::triggered, this, &MainWindow::next);

    connect(m_zoomin_act, &QAction::triggered, this, &MainWindow::zoomIn);
    connect(m_zoomout_act, &QAction::triggered, this, &MainWindow::zoomOut);
    connect(m_zoomfit_act, &QAction::triggered, this, &MainWindow::zoomFit);
    connect(m_zoomfull_act, &QAction::triggered, this, &MainWindow::zoomFull);

    // TODO: implement next/previous function
    m_prev_act->setDisabled(true);
    m_next_act->setDisabled(true);
}

MainWindow::~MainWindow()
{
}

void MainWindow::imageChanged(const QImage *i)
{
    if (!i->isNull())
    {
        QString wt = QString("%1 (%2x%3) (%4%)").arg(QFileInfo(m_currentpath).fileName(),
                                                     QString::number(i->width()),
                                                     QString::number(i->height()),
                                                     QString::number(qFloor(m_view->zoom() * 100)));
        setWindowTitle(wt);
    }
}

void MainWindow::zoomChanged(qreal)
{
    imageChanged(m_view->image());
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls() &&
        (e->mimeData()->urls().length() == 1) &&
        (pathIsImage(e->mimeData()->urls().first().toLocalFile())))
    {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    e->accept();
    setCurrentPath(e->mimeData()->urls().first().toLocalFile());
}

const QString &MainWindow::currentPath(void)
{
    return m_currentpath;
}

void MainWindow::setCurrentPath(const QString &path)
{
    auto i = new QImage(path);

    if (i->format() != QImage::Format_Invalid)
    {
        m_currentpath = path;
        m_view->setImage(i);
    }
    else
    {
        delete i;
    }
}

bool MainWindow::pathIsImage(const QString &path)
{
    bool is_image = false;

    QMimeType pathMimeType = m_mimedatabase->mimeTypeForFile(path, QMimeDatabase::MatchExtension);

    for (QStringList::ConstIterator it = m_accepted_types.begin(); it != m_accepted_types.end(); it++)
    {
        if (pathMimeType.inherits(*it))
        {
            is_image = true;
            break;
        }
    }

    return is_image;
}

void MainWindow::zoomIn()
{
    m_view->setZoomHint(ImageView::Absolute);
    qreal z_mult = (m_view->zoom() + m_zoomadj) / m_zoomadj;
    qreal new_z = qFloor(z_mult) * m_zoomadj;

    m_view->setZoom(new_z);
}

void MainWindow::zoomOut()
{
    m_view->setZoomHint(ImageView::Absolute);
    qreal z_mult = (m_view->zoom() - m_zoomadj) / m_zoomadj;
    qreal new_z = qCeil(z_mult) * m_zoomadj;

    if (new_z < 0.25)
    {
        new_z = 0.25;
    }

    m_view->setZoom(new_z);
}

void MainWindow::zoomFit()
{
    m_view->setZoomHint(ImageView::Fit);
}

void MainWindow::zoomFull()
{
    m_view->setZoomHint(ImageView::Isometric);
}

void MainWindow::next()
{
}

void MainWindow::prev()
{
}

void MainWindow::open()
{
    QFileDialog *dialog = new QFileDialog(this, tr("Open Image"));
    QString extensions;
    QList<QMimeType> mimeList;

    for (QStringList::ConstIterator it = m_accepted_types.begin();
         it != m_accepted_types.end();
         it++)
    {
        QMimeType t = m_mimedatabase->mimeTypeForName(*it);
        if (t.isValid())
        {
            extensions.append(" ");
            extensions.append(t.globPatterns().join(" "));
        }
    }

    extensions.prepend("Supported Image Types (");
    extensions.append(")");

    dialog->setDirectory(m_currentdir);
    dialog->setNameFilter(extensions);
    dialog->setAcceptMode(QFileDialog::AcceptOpen);
    dialog->setFileMode(QFileDialog::ExistingFile);

    // FIXME: the documentation recommends avoiding use of the exec() member function
    //        of QDialog subclasses, instead favoring a open() and connecting up
    //        the finished() signal to a slot in the parent. This is said to avoid bugs
    //        when the parent widget of the dialog is deleted, but as the parent of this
    //        dialog is a main window, i think it's safe enough. caveat emptor
    int result = dialog->exec();

    if (result == QFileDialog::Accepted)
    {
        setCurrentPath(dialog->selectedFiles().first());
        m_currentdir = dialog->directory();
    }

    delete dialog;
}
