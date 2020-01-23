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

#include "imageview.h"
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTimer>
#include <QtMath>
#include <limits>

ImageView::ImageView(QWidget *p):
    QAbstractScrollArea(p),
    m_image(nullptr),
    m_resizetimer(new QTimer(this)),
    m_zoom(1.0),
    m_zoomhint(ImageView::Isometric)
{
    connect(m_resizetimer, &QTimer::timeout, this, &ImageView::resizeTimeout);
}

ImageView::~ImageView()
{
    delete m_image;
    delete m_resizetimer;
}

const QImage *ImageView::image() const
{
    return m_image;
}

qreal ImageView::zoom() const
{
    return m_zoom;
}

ImageView::ZoomHint ImageView::zoomHint() const
{
    return m_zoomhint;
}

void ImageView::setImage(const QImage *i)
{
    delete m_image;
    m_image = const_cast<QImage *>(i);
    horizontalScrollBar()->setValue(0);
    verticalScrollBar()->setValue(0);
    m_updateScrollBars();
    viewport()->update();
    emit imageChanged(m_image);
}

qreal ImageView::m_zoomFitFactor()
{
    qreal z = 1.0;

    if (image())
    {
        // Check scale by width first.
        z = qreal(viewport()->width()) / qreal(image()->width());

        // Scaled by width, height is still outside of bounds of viewport.
        if (qFloor(image()->height() * z) > viewport()->height())
        {
            z = qreal(viewport()->height()) / qreal(image()->height());
        }
    }

    return z;
}

void ImageView::setZoom(qreal z)
{
    if ((z > 0.0) && image())
    {
        auto new_x = qFloor(horizontalScrollBar()->value() / zoom() * z);
        auto new_y = qFloor(verticalScrollBar()->value() / zoom() * z);

        m_zoom = z;

        m_updateScrollBars();

        viewport()->update();

        horizontalScrollBar()->setValue(new_x);
        verticalScrollBar()->setValue(new_y);

        emit zoomChanged(z);
    }
}

void ImageView::setZoomHint(ImageView::ZoomHint h)
{
    if (zoomHint() != h)
    {
        qreal z = zoom();

        switch(h)
        {
            case Isometric:
                z = 1.0;
                break;
            case Fit:
                z = m_zoomFitFactor();
                break;
            case Absolute:
                break;
        }

        m_zoomhint = h;

        emit zoomHintChanged(h);

        if (qFabs(zoom() - z) > std::numeric_limits<qreal>::epsilon())
        {
            setZoom(z);
        }
    }
}

void ImageView::m_updateScrollBars()
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

    p.fillRect(e->rect(), Qt::black);

    if (image())
    {
        QRect s_rect = e->rect();

        s_rect.moveTopLeft(QPoint(qFloor(horizontalScrollBar()->value() / zoom()),
                                  qFloor(verticalScrollBar()->value() / zoom())));
        s_rect.setSize(s_rect.size() / zoom());

        QRect d_rect = e->rect();

        auto d_width_diff = d_rect.width() - image()->width() * zoom();
        auto d_height_diff = d_rect.height() - image()->height() * zoom();

        if (d_width_diff > 0)
        {
            d_rect.moveLeft(qFloor(d_width_diff/2));
        }
        if (d_height_diff > 0)
        {
            d_rect.moveTop(qFloor(d_height_diff/2));
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
    if (zoomHint() == ImageView::Fit)
    {
        setZoom(m_zoomFitFactor());
    }

    if (image())
    {
        m_updateScrollBars();
    }
}
