#include <QtGui/QMovie>
#include <QtGui/QPixmap>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->generateButton->setHidden(true);

    connect(ui->inputUpdatesite, SIGNAL(textChanged(QString)), this, SLOT(slotUpdatesiteChanged(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotUpdatesiteValid()
{
    ui->icon->setPixmap(QPixmap(QString::fromUtf8(":/valid")));
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::slotUpdatesiteInvalid()
{
    ui->icon->setPixmap(QPixmap(QString::fromUtf8(":/not_valid")));
}

void MainWindow::slotUpdatesiteLoading()
{
    QMovie * animation = new QMovie(QString::fromUtf8(":/loading"), QByteArray(), this);
    animation->setScaledSize(QSize(75,75));
    animation->start();
    ui->icon->setMovie(animation);
}

void MainWindow::slotUpdatesiteChanged(QString updateSite)
{
    emit updatesiteChanged(updateSite);
}
