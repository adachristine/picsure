#include "imageview.h"
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTimer>
#include <QtMath>

ImageView::ImageView(QWidget *p):
    QAbstractScrollArea(p),
    m_image(nullptr),
    m_resizetimer(new QTimer(this)),
    m_zoom(1.0)
{
    connect(m_resizetimer, &QTimer::timeout, this, &ImageView::resizeTimeout);
}

ImageView::~ImageView()
{
    delete m_image;
    delete m_resizetimer;
}

const QImage *ImageView::image()
{
    return m_image;
}

void ImageView::setImage(QImage *i)
{
    delete m_image;
    m_image = i;

    if(m_zoomfit)
    {
        setZoomFit();
    }
    else
    {
        setZoom(m_zoom);
    }

    emit imageChanged(m_image);
}

void ImageView::setZoomFit()
{
    qreal z;

    if (image())
    {
        // Check scale by width first.
        z = qreal(viewport()->width()) / qreal(image()->width());

        // Scaled by width, height is still outside of bounds of viewport.
        if (qFloor(image()->height() * z) > viewport()->height())
        {
            z = qreal(viewport()->height()) / qreal(image()->height());
        }

        setZoom(z);
    }

    m_zoomfit = true;
}

void ImageView::setZoomFull()
{
    setZoom(1.0);
}

qreal ImageView::zoom()
{
    return m_zoom;
}

void ImageView::setZoom(qreal z)
{
    if (z > 0.0)
    {
        auto new_x = qFloor(horizontalScrollBar()->value() / zoom() * z);
        auto new_y = qFloor(verticalScrollBar()->value() / zoom() * z);

        m_zoom = z;

        updateScrollBars();

        viewport()->update();

        horizontalScrollBar()->setValue(new_x);
        verticalScrollBar()->setValue(new_y);

        m_zoomfit = false;

        emit zoomChanged(m_zoom);
    }
}

void ImageView::updateScrollBars()
{
    if (image())
    {
        horizontalScrollBar()->setRange(0, qFloor(image()->width() * zoom()) - viewport()->width());
        verticalScrollBar()->setRange(0, qFloor(image()->height() * zoom()) - viewport()->height());
    }
    else
    {
        horizontalScrollBar()->setRange(0, 0);
        verticalScrollBar()->setRange(0, 0);
    }
}

void ImageView::paintEvent(QPaintEvent *e)
{
    QPainter p(viewport());

    QAbstractScrollArea::paintEvent(e);

    p.fillRect(e->rect(), Qt::gray);

    if (image())
    {
        QRect s_rect = e->rect();

        s_rect.moveTopLeft(QPoint(qFloor(horizontalScrollBar()->value() / zoom()),
                                  qFloor(verticalScrollBar()->value() / zoom())));
        s_rect.setSize(s_rect.size() / zoom());

        QRect d_rect = e->rect();

        if (d_rect.width() - (image()->width() * zoom()) > 0)
        {
            d_rect.moveLeft((d_rect.width() - qFloor(image()->width() * zoom()))/2);
        }
        if (d_rect.height() - image()->height() * zoom() > 0)
        {
            d_rect.moveTop((d_rect.height() - qFloor(image()->height() * zoom()))/2);
        }

        p.drawImage(d_rect, *image(), s_rect);
    }

    e->accept();
}

void ImageView::resizeEvent(QResizeEvent *e)
{

    QAbstractScrollArea::resizeEvent(e);

    // Only resize after the timeout of no resize events.
    m_resizetimer->start(m_timeout_msec);

    e->accept();
}

void ImageView::resizeTimeout()
{
    if (m_zoomfit)
    {
        setZoomFit();
    }

    if (image())
    {
        updateScrollBars();
    }
}
