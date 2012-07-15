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

void MainWindow::slotUpdatesiteChanged(QString updateSite)
{
    emit updatesiteChanged(updateSite);
}
