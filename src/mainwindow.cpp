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
}

void MainWindow::slotUpdatesiteInvalid()
{
    ui->icon->setPixmap(QPixmap(QString::fromUtf8(":/not_valid")));
}

void MainWindow::slotUpdatesiteLoading()
{
    QPixmap pixmap(QString::fromUtf8(":/loading"));
    ui->icon->setPixmap(pixmap.scaled(75,75, Qt::KeepAspectRatio));
}

void MainWindow::slotUpdatesiteChanged(QString updateSite)
{
    emit updatesiteChanged(updateSite);
}
