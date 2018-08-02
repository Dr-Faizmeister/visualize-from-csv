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

const QString allFileToString(QFile &aFile)
{
/*    if (!aFile.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox msgBox;
        msgBox.setText("Error opening file!");
        msgBox.exec();
        return nullptr;
    }*/
    QTextStream in(&aFile);
    return in.readAll();
}

void setDataToVector(const QStringList &aStringList,
                     QVector< QVector <double> > &aVector)
{
    int rowCount = aStringList.size() - 2; // row count
    int colCount = aStringList.at(0).count(";") + 1; // col count

    for (int i = 0; i < rowCount; ++i) {
        QVector<double> temp_vector;
        for (int j = 0; j < colCount; ++j) {
            temp_vector.push_back(aStringList.at(i+1).split(";").at(j).toDouble());
        }
        aVector.push_back(temp_vector);
    }
}

void MainWindow::on_btnHide_clicked()
{
    chrt->removeAllSeries();
}

void MainWindow::on_btnShow_clicked()
{
    minX = ui->lineEditMin->text().toDouble();
    maxX = ui->lineEditMax->text().toDouble();

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
    QMessageBox msgBox;
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"), ":/", tr("CSV Files (*.csv)"));
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

//    QString firstRow = QString::fromUtf8(file.readLine());
    QString firstRow = file.readLine();
    headers = firstRow.split(';');
    headers.removeAt(0);

    QStringList firstColumn;
    while (!file.atEnd()) {
    QByteArray line = file.readLine();
    firstColumn.append(line.split(';').first());

    }

    data.squeeze();
    setDataToVector(allFileToString(file).split("\n"), data);

// constructing complex array
    for (int j = 1; j < firstRow.size(); j++) {
        QVector < QVector <double> > temp2D;
        QVector <double> tempValue;
        QVector <double> tempDepth;
        for (int i = 0; i < data.size(); i++)
        {
            if (data.at(i).at(j) != -999.25)
            {
                tempDepth.push_back(data.at(i).at(0));
                tempValue.push_back(data.at(i).at(j));
            }
        }
        temp2D.push_back(tempDepth);
        temp2D.push_back(tempValue);

        array.push_back(temp2D);
    }

// max/min
    for (int i = 0; i < array.size(); i++)
    {
        double maxX, minX;
        minimalDepth.push_back(array.at(i).at(0).at(0));
        msgBox.setText("minimalDepth");
        msgBox.exec();
        maximalDepth.push_back(array.at(i).at(0).at(array.at(i).at(0).size()));

        maxX = -100000;
        minX = 100000;

        for (int j = 0; j < array.at(i).at(0).size(); j++)
        {
            msgBox.setText("maxElement cycle");
            msgBox.exec();
            if (maxX < array.at(i).at(1).at(j)) { maxX = array.at(i).at(1).at(j); }
        }
        maximalX.push_back(maxX);
        for (int jj = 0; jj < array.at(i).at(0).size(); jj++)
        {
            if (minX > array.at(i).at(1).at(jj)) { minX = array.at(i).at(1).at(jj); }
        }
        minimalX.push_back(minX);
    }

// GIS-method's list
    for (int i = 0; i < headers.size(); i++)
    {
        headers[i] = headers[i] + " (" + QString::number(minimalDepth.at(i)) + " - " + QString::number(maximalDepth.at(i)) + ")";
    }
    ui->listWidget->addItems(headers);
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
//    int currentRow = ui->listWidget->currentRow();
    QString itemName = item->text();
    ui->lblGisMethod->setText(itemName);
//    QStandardItemModel model(data.size(), 2);
/*    array.squeeze();
    int a, b;
    for (int i = 0; i < data.size(); i++)
        for (int j = 0; j < data[i].size(); j++)
        {
            if (data[i][j] != -999.25)
            {
                array[a][b] = data[i][j];
                a++;
                b++;
            }
        }*/
}
