#include "mainwindow.h"
#include "imageview.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QKeyEvent>
#include <QFileInfo>
#include <QtMath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_view(new ImageView(this))
{
    setCentralWidget(m_view);
    setAcceptDrops(true);
    resize(800,600);
    m_view->setZoomHint(ImageView::Fit);

    connect(m_view, &ImageView::imageChanged, this, &MainWindow::imageChanged);
    connect(m_view, &ImageView::zoomChanged, this, &MainWindow::zoomChanged);
}

MainWindow::~MainWindow()
{
    delete m_view;
}

void MainWindow::imageChanged(const QImage *i)
{
    if (!i->isNull())
    {
        QString wt = QString("%1 (%2x%3) (%4%)").arg(QFileInfo(m_currentpath).baseName(),
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
    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    if (e->mimeData()->urls().length() > 0)
    {
        QString path = e->mimeData()->urls().at(0).toLocalFile();
        QFileInfo f(path);
        if (f.exists() && f.isFile())
        {
            QImage *i = new QImage(path);
            m_currentpath = path;
            m_view->setImage(i);
        }

        e->accept();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (!e->isAutoRepeat())
    {
        if (e->matches(QKeySequence::ZoomIn))
        {
            m_view->setZoom(m_view->zoom() + 0.25);
        }
        else if (e->matches(QKeySequence::ZoomOut))
        {
            m_view->setZoom(m_view->zoom() - 0.25);
        }

        e->accept();
    }
    else
    {
        QMainWindow::keyPressEvent(e);
    }
}
