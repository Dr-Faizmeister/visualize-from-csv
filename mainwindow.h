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
    QPen pen;
    double minX, maxX, minY, maxY;
    int currentRow;
    QString fileName;
    QVector < QVector <double> > data;
    QStringList headers;
    QStringList horizHeader;
    QVector < QVector < QVector <double> > > array;
    QVector <double> depth;
    QVector <double> minimalX;
    QVector <double> maximalX;
    QVector <double> minimalDepth;
    QVector <double> maximalDepth;

    struct Reading
    {
        double depth;
        double value;
        Reading(const double p_depth, const double p_value)
            : depth(p_depth), value(p_value) {}
    };

    std::vector < std::vector <Reading> > mainData;
};

#endif // MAINWINDOW_H
