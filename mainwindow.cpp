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
    minX = ui->lineEditMin->text().toDouble();
    maxX = ui->lineEditMax->text().toDouble();

    minY = mainData[currentRow][0].depth;
    maxY = mainData[currentRow][mainData[currentRow].size()].depth;

    chrt = new QChart;

    chrt->legend()->hide();

    ui->vchrt->setChart(chrt);
    ui->vchrt->setRubberBand(QChartView::VerticalRubberBand);

    QLineSeries* series1 = new QLineSeries;
    pen.setWidthF(0.5);
    pen.setBrush(Qt::red);
    series1->setPen(pen);

    for (int i = 0; i < mainData[currentRow].size(); i++)
    {
        series1->append(mainData[currentRow][i].value, mainData[currentRow][i].depth);
    }

    chrt->addSeries(series1);

    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(minX, maxX);
    axisX->setMinorTickCount(6);
    axisX->setTickCount(3);
    axisX->setLabelFormat("%.2f");
    axisX->setTitleText(headers[currentRow]);
    chrt->addAxis(axisX, Qt::AlignTop);
    series1->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setReverse();
    axisY->setRange(minY, maxY);
    axisY->setMinorTickCount(10);
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

    QString firstRow = file.readLine();
    headers = firstRow.split(';');
    headers.removeAt(0);

    QStringList firstColumn;
    QStringList lines;
    while (!file.atEnd()) {
    QByteArray line = file.readLine();
    firstColumn.append(line.split(';').first());
    lines.append(line);
    }
    data.squeeze();

    int rowCount = lines.size() + 1; // row count
    int colCount = lines.at(0).count(";") + 2; // col count

    for (int i = 0; i < rowCount - 1; i++) {
        QVector<double> temp_vector;
        for (int j = 0; j < colCount - 1; j++) {
            temp_vector.push_back(lines.at(i).split(";").at(j).toDouble());
        }
        data.push_back(temp_vector);
        temp_vector.squeeze();
    }

// data-out test
/*    QString output_file_name = "out.txt";
    QFile output_file(output_file_name);
    if (!output_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        msgBox.setText("Unable to write into output file!");
        msgBox.exec();
    }
    QTextStream out(&output_file);
    for (int i = 0; i < rowCount - 1; i++)
    {
        for (int j = 0; j < colCount - 1; j++)
        {
            out << data.at(i).at(j) << "\t";
        }
        out << "\n";
    }
    output_file.close();
*/

// constructing complex array
    for (int j = 1; j < colCount - 1; j++)
    {
        std::vector <Reading> readings;
        double p_depth;
        double p_value;
        for (int i = 0; i < rowCount - 1; i++)
        {
            if (data.at(i).at(j) != -999.25)
            {
                p_depth = data.at(i).at(0);
                p_value = data.at(i).at(j);
                readings.emplace_back(p_depth, p_value);
            }
/*            else
            {
                readings.emplace_back(0, 0);
            }*/
        }
        if (readings.size() == 0) { readings.emplace_back(0, 0); }
        mainData.emplace_back(readings);
        readings.clear();
    }

// max/min
    for (size_t i = 0; i < mainData.size(); i++)
    {
        double maxX, minX, minDepth, maxDepth;
        minDepth = mainData.at(i).at(0).depth;
        maxDepth = mainData.at(i).at(mainData.at(i).size() - 1).depth;
        minimalDepth.push_back(minDepth);
        maximalDepth.push_back(maxDepth);

        maxX = -100000;
        minX = 100000;

        for (size_t j = 0; j < mainData.at(i).size() - 1; j++)
        {
            if (maxX < mainData.at(i).at(j).value) { maxX = mainData.at(i).at(j).value; }
        }
        maximalX.push_back(maxX);

        for (size_t jj = 0; jj < mainData.at(i).size() - 1; jj++)
        {
            if (minX > mainData.at(i).at(jj).value) { minX = mainData.at(i).at(jj).value; }
        }
        minimalX.push_back(minX);
    }

// GIS-method's list
    for (int i = 0; i < mainData.size(); i++)
    {
        headers[i] = headers[i] + " (" + QString::number(minimalDepth.at(i)) + " - " + QString::number(maximalDepth.at(i)) + ")";
    }
    ui->listWidget->addItems(headers);

    horizHeader.append("Глубина");
    horizHeader.append("Значение");
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    currentRow = ui->listWidget->currentRow();
    QString itemName = item->text();
    ui->lblGisMethod->setText(itemName);
    ui->lineEditMin->setText(QString::number(minimalX.at(currentRow)));
    ui->lineEditMax->setText(QString::number(maximalX.at(currentRow)));
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setRowCount(mainData[currentRow].size());
    ui->tableWidget->setHorizontalHeaderLabels(horizHeader);

    for (int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        QTableWidgetItem *twItem = new QTableWidgetItem();
        twItem->setText(QString::number(mainData.at(currentRow).at(i).depth)/*.arg(i).arg(0)*/);
        ui->tableWidget->setItem(i, 0, twItem);
    }

    for (int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        QTableWidgetItem *twItem = new QTableWidgetItem();
        twItem->setText(QString::number(mainData.at(currentRow).at(i).value)/*.arg(i).arg(1)*/);
        ui->tableWidget->setItem(i, 1, twItem);
    }

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
