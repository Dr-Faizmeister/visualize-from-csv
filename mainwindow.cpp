#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)),
                this, SLOT(on_listWidget_itemClicked(QListWidgetItem*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnHide_clicked()
{
    chrt->removeAllSeries();
}

void MainWindow::on_btnShow_clicked()
{
    chrt = new QChart;

    chrt->legend()->hide();

    ui->vchrt->setChart(chrt);
    ui->vchrt->setRubberBand(QChartView::VerticalRubberBand);

    QLineSeries* series1 = new QLineSeries;

    for (int i = 0; i < 200; i++)
        series1->append(qSin(2*M_PI*i/25), i);

    chrt->addSeries(series1);

    chrt->setTitle("Кривая ГИС");

    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(minX, maxX);
    axisX->setTickCount(3);
    axisX->setLabelFormat("%.2f");
    axisX->setTitleText("Значение");
    chrt->addAxis(axisX, Qt::AlignTop);
    series1->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(minY, maxY);
    axisY->setTickCount(5);
    axisY->setLabelFormat("%g");
    axisY->setTitleText("Глубина, м");
    chrt->addAxis(axisY, Qt::AlignLeft);
    series1->attachAxis(axisY);
}

void MainWindow::on_btnOpen_clicked()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"), ":/", tr("CSV Files (*.csv)"));
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

//    QString firstRow = QString::fromUtf8(file.readLine());
    QString firstRow = file.readLine();
    QStringList headers = firstRow.split(';');
    ui->listWidget->addItems(headers);
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    int currentRow = ui->listWidget->currentRow();
    QString itemName = item->text();
    ui->lblGisMethod->text() = itemName;
}
