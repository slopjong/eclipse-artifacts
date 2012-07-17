#ifndef RESIZEANIMATION_H
#define RESIZEANIMATION_H

#include <QtCore/QTimer>

class MainWindow;

class ResizeAnimation : public QTimer
{
    Q_OBJECT
public:
    explicit ResizeAnimation(MainWindow *window, QObject *parent = 0);
    ~ResizeAnimation();

private:

    MainWindow * m_window;

    void decreaseHeight();
    void increaseHeight();

    bool m_hitZero;

signals:
    
public slots:
    void start();
    void stop();

private slots:
    void oneSecondWaited();
    void doTransition();
    
};

#endif // RESIZEANIMATION_H
