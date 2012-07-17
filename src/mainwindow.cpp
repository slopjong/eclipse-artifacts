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
    connect(ui->generateButton, SIGNAL(clicked()), this, SLOT(slotGenerateButtonClicked()));
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

void MainWindow::slotShowGenerateButton()
{
    ui->progressBar->hide();
    ui->fileDownloading->hide();
    ui->generateButton->show();
    ui->generateButton->setEnabled(true);
}

void MainWindow::slotProgressChanged(int progress)
{
    int max = ui->progressBar->maximum();
    int val = ui->progressBar->value();
    ui->progressBar->setValue(progress);
    QString label = QString("Downloaded %1 artifacts of %2.").arg(val).arg(max);
    ui->fileDownloading->setText(label);
}

void MainWindow::slotProgressMaxChanged(int max)
{
    ui->progressBar->setMaximum(max);
}

void MainWindow::slotFileDownloading(QString fileName)
{
    ui->fileDownloading->setText(fileName.prepend("Downloading "));
}

void MainWindow::initInput(QHash<QString, QString> input)
{
    ui->iPkgrel->setText(input.value("PKGREL"));
    ui->iEclipsever->setText(input.value("ECLIPSEVER"));
    ui->iLicense->setText(input.value("LICENSE"));
}

void MainWindow::slotUpdatesiteChanged(QString updateSite)
{
    emit updatesiteChanged(updateSite);
}

void MainWindow::slotGenerateButtonClicked()
{
    QHash<QString, QString> input;

    input.insert("MAINTAINER", ui->iMaintainer->text());
    input.insert("EMAIL", ui->iEmail->text());
    input.insert("PKGNAME", ui->iPkgname->text());
    input.insert("PKGVER", ui->iPkgver->text());
    input.insert("PKGREL", ui->iPkgrel->text());
    input.insert("DESCRIPTION", ui->iPkgdesc->text());
    input.insert("URL", ui->iUrl->text());
    input.insert("ECLIPSEVER", ui->iEclipsever->text());
    input.insert("LICENSE", ui->iLicense->text());
    input.insert("DEPENDS", ui->iDeps->text());

    emit inputChanged(input);
    emit generatePkgbuild();
}
