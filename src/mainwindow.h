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

private slots:

    void slotUpdatesiteChanged(QString updateSite);

signals:
    void updatesiteChanged(QString updateSite);

};

#endif // MAINWINDOW_H
