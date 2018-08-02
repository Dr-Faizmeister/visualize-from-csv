#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include <QVector>
#include <sstream>
#include <QTextCodec>
#include <QMessageBox>

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
    QVector < QVector <double> > data;
    QStringList headers;
    QVector < QVector < QVector <double> > > array;
    QVector <double> depth;
    QVector <double> minimalX;
    QVector <double> maximalX;
    QVector <double> minimalDepth;
    QVector <double> maximalDepth;
};

#endif // MAINWINDOW_H
