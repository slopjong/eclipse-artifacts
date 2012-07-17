#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QHash>
#include <QtCore/QTimer>
#include <QtGui/QMainWindow>

#include "ui_mainwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    //friend class ResizeAnimation;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void initInput(QHash<QString, QString> input);
    
private:
    Ui::MainWindow *ui;

    QTimer *resizeTimer;

public slots:
    void slotUpdatesiteValid();
    void slotUpdatesiteInvalid();
    void slotUpdatesiteLoading();
    void slotShowGenerateButton();
    void slotProgressChanged(int progress);
    void slotProgressMaxChanged(int max);
    void slotFileDownloading(QString fileName);

private slots:

    void slotUpdatesiteChanged(QString updateSite);
    void slotGenerateButtonClicked();
    void slotWaitOnValid();
    void slotResize();

signals:
    void updatesiteChanged(QString updateSite);
    void generatePkgbuild();
    void inputChanged(QHash<QString, QString>);
};

#endif // MAINWINDOW_H
