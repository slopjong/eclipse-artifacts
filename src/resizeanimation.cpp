#include "resizeanimation.h"
#include "mainwindow.h"

#include <QDebug>

ResizeAnimation::ResizeAnimation(MainWindow *window, QObject *parent) :
    QTimer(parent),
    m_window(window),
    m_hitZero(false)
{
}

ResizeAnimation::~ResizeAnimation()
{
    //QTimer::~QTimer();
}

void ResizeAnimation::decreaseHeight()
{
    // the current size
    QSize s(m_window->size());

    // decrement the height
    s.setHeight(s.height() - 1);

    m_window->resize(s);

    if( s.height() == 0)
        m_hitZero = true;
}

void ResizeAnimation::increaseHeight()
{
    // the current size
    QSize s(m_window->size());

    // increment the height
    s.setHeight(s.height() + 1);

    m_window->resize(s);

    if( s.height() == m_window->sizeHint().height() )
        stop();
}

void ResizeAnimation::doTransition()
{
    // The slide out transition doesn't work because some elements
    // have a minimum size set and at some point the window stops to
    // resize and doesn't get smaller. So we skip the decreaseHeight()

    m_hitZero = true;

    if(!m_hitZero)
    {
        decreaseHeight();
    }
    else
    {
        m_window->ui->stackedWidget->setCurrentIndex(1);;
        increaseHeight();
    }
}

void ResizeAnimation::oneSecondWaited()
{
    connect(this, SIGNAL(timeout()), this, SLOT(doTransition()));
    QTimer::start(5);
}

void ResizeAnimation::start()
{
    singleShot(1000, this, SLOT(oneSecondWaited()));
}

void ResizeAnimation::stop()
{
    QTimer::stop();
    deleteLater();
}

