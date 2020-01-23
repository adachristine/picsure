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

#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QAbstractScrollArea>

class QImage;
class QTimer;

class ImageView : public QAbstractScrollArea
{
    Q_OBJECT
    Q_PROPERTY(const QImage *image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
    Q_PROPERTY(ZoomHint zoomHint READ zoomHint WRITE setZoomHint NOTIFY zoomHintChanged)

public:
    enum ZoomHint {Absolute, Isometric, Fit};
    Q_ENUM(ZoomHint)

    ImageView(QWidget *p);
    ImageView(QWidget *p, QImage *image);
    ~ImageView();

    const QImage *image() const;
    qreal zoom() const;
    ZoomHint zoomHint() const;

public slots:
    void setZoom(qreal z);
    void setImage(QImage *i);
    void setZoomHint(ZoomHint h);

signals:
    void zoomChanged(qreal z);
    void zoomHintChanged(ZoomHint z);
    void imageChanged(const QImage *i);

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);

private:
    static const int m_timeout_msec = 150;

    QImage *m_image;
    QTimer *m_resizetimer;
    qreal m_zoom;
    ZoomHint m_zoomhint;

    void m_updateScrollBars();
    qreal m_zoomFitFactor();

private slots:
    void resizeTimeout();
};

#endif // IMAGEVIEW_H
