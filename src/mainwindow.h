#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QHash>
#include <QtGui/QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void initInput(QHash<QString, QString> input);
    
private:
    Ui::MainWindow *ui;

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

signals:
    void updatesiteChanged(QString updateSite);
    void generatePkgbuild();
    void inputChanged(QHash<QString, QString>);
};

#endif // MAINWINDOW_H
