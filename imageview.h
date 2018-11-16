#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QAbstractScrollArea>

class QImage;
class QTimer;

class ImageView : public QAbstractScrollArea
{
    Q_OBJECT

public:
    ImageView(QWidget *p = nullptr);
    ~ImageView();

    const QImage *image();
    void setImage(QImage *i);

    qreal zoom();
    void setZoom(qreal z);
    void setZoomFit();
    void setZoomFull();

    static const int m_timeout_msec = 150;

signals:
    void zoomChanged(qreal z);
    void imageChanged(const QImage *i);

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);

private:
    QImage *m_image;
    QTimer *m_resizetimer;
    qreal m_zoom;
    bool m_zoomfit;

    void updateScrollBars();

private slots:
    void resizeTimeout();
};

#endif // IMAGEVIEW_H
