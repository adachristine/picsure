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
    ~ImageView();

    const QImage *image() const;
    qreal zoom() const;
    ZoomHint zoomHint() const;

public slots:
    void setZoom(qreal z);
    void setImage(const QImage *i);
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
