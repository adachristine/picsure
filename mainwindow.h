#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class ImageView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void imageChanged(const QImage *i);
    void zoomChanged(qreal z);

protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
    void keyPressEvent(QKeyEvent *e);

private:
    ImageView *m_view;
    QString m_currentpath;
};



#endif // MAINWINDOW_H
