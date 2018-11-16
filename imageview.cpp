#include "imageview.h"
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTimer>

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

    // TODO: figure out how to stop the oscillating
    if (m_image)
    {
        if (m_image->width() > m_image->height())
        {
            setZoom(qreal(viewport()->width()) / qreal(m_image->width()));
        }
        else
        {
            setZoom(qreal(viewport()->height() / qreal(m_image->height())));
        }

        m_zoomfit = true;
    }
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
        int new_x = int(horizontalScrollBar()->value() / m_zoom * z);
        int new_y = int(verticalScrollBar()->value() / m_zoom * z);

        m_zoom = z;

        updateScrollBars();

        horizontalScrollBar()->setValue(new_x);
        verticalScrollBar()->setValue(new_y);

        viewport()->update();

        m_zoomfit = false;

        emit zoomChanged(m_zoom);
    }
}

void ImageView::updateScrollBars()
{
    if (m_image)
    {
        horizontalScrollBar()->setRange(0, int(m_image->width() * m_zoom) - viewport()->width());
        verticalScrollBar()->setRange(0, int(m_image->height() * m_zoom) - viewport()->height());
    }
}

void ImageView::paintEvent(QPaintEvent *e)
{
    QPainter p(viewport());

    QAbstractScrollArea::paintEvent(e);

    p.fillRect(e->rect(), Qt::gray);

    if (m_image)
    {
        QRect s_rect(int(horizontalScrollBar()->value() / m_zoom),
                     int(verticalScrollBar()->value() / m_zoom),
                     int(e->rect().width() / m_zoom),
                     int(e->rect().height() / m_zoom));

        QRect d_rect = e->rect();

        if (d_rect.width() - (m_image->width() * m_zoom) > 0)
        {
            d_rect.moveLeft((d_rect.width() - int(m_image->width() * m_zoom))/2);
        }
        if (d_rect.height() - m_image->height() * m_zoom > 0)
        {
            d_rect.moveTop((d_rect.height() - int(m_image->height() * m_zoom))/2);
        }

        p.drawImage(d_rect, *m_image, s_rect);
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
    if (m_image)
    {
        updateScrollBars();
    }

    if (m_zoomfit)
    {
        setZoomFit();
    }
}
