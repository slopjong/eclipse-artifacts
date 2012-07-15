#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;

public slots:
    void slotUpdatesiteValid();
    void slotUpdatesiteInvalid();
    void slotUpdatesiteLoading();
    void slotShowGenerateButton();

private slots:

    void slotUpdatesiteChanged(QString updateSite);
    void slotGenerateButtonClicked();

signals:
    void updatesiteChanged(QString updateSite);
    void generatePkgbuild();

};

#endif // MAINWINDOW_H
