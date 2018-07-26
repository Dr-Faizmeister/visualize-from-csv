#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include <sstream>
#include <QTextCodec>

using namespace QtCharts;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnHide_clicked();

    void on_btnShow_clicked();

    void on_btnOpen_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;

    QChart *chrt;
    double minX, maxX, minY, maxY;
    QString fileName;
};

#endif // MAINWINDOW_H
