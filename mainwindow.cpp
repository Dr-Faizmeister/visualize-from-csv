#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)),
                        this, SLOT(on_listWidget_itemClicked(QListWidgetItem*)));

    QMessageBox msgBox;
    chrt = new QChart;
    chrt->legend()->hide();
    ui->vchrt->setChart(chrt);
    ui->vchrt->setRubberBand(QChartView::VerticalRubberBand);

    brushVector << Qt::red << Qt::gray << Qt::green << Qt::black << Qt::blue << Qt::darkBlue
                << Qt::darkCyan << Qt::darkGray << Qt::darkGreen << Qt::darkMagenta << Qt::darkRed;

    axisY.setReverse();
    axisY.setRange(0, 0);
    axisY.setMinorTickCount(10);
    axisY.setTickCount(5);
    axisY.setLabelFormat("%g");
    axisY.setTitleText("Глубина, м");
    chrt->addAxis(&axisY, Qt::AlignLeft);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnHide_clicked()
{
//    series[currentRow]->detachAxis(axisXs[currentRow]);
    chrt->removeSeries(series[currentRow]);
    chrt->removeAxis(axisXs[currentRow]);
}

void MainWindow::on_btnShow_clicked()
{
    minX = ui->lineEditMin->text().toDouble();
    maxX = ui->lineEditMax->text().toDouble();

    minY = mainData[currentRow][0].depth;
    maxY = mainData[currentRow][mainData[currentRow].size()].depth;

    chrt->addSeries(series[currentRow]);

    chrt->addAxis(axisXs[currentRow], Qt::AlignTop);
    series[currentRow]->attachAxis(axisXs[currentRow]);

    axisXs[currentRow]->setRange(minX, maxX);
    series[currentRow]->attachAxis(&axisY);

}

void MainWindow::on_btnOpen_clicked()
{
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
        headersOnly.append(headers[i]);
        headers[i] = headers[i] + " (" + QString::number(minimalDepth.at(i)) + " - " + QString::number(maximalDepth.at(i)) + ")";
    }
    ui->listWidget->addItems(headers);

    horizHeader.append("Глубина");
    horizHeader.append("Значение");

// collecting chart series

//    series = new QLineSeries*[mainData.size()];
    axisXs = new QValueAxis*[mainData.size()];

    for (int i = 0; i < mainData.size(); i++)
    {
        int brushVectorIndex = rand()%brushVector.size();
        QLineSeries* serie = new QLineSeries();
        pen.setWidthF(1);
        pen.setBrush(brushVector.at(brushVectorIndex));
        serie->setPen(pen);

        for (int j = 0; j < mainData[i].size(); j++)
        {
            *serie << QPointF(mainData[i][j].value, mainData[i][j].depth);
        }
        connect(serie, &QXYSeries::clicked, this, &MainWindow::on_series_Clicked);
        series << serie;

        QValueAxis *axisXItem = new QValueAxis;
        axisXItem->setMinorTickCount(6);
        axisXItem->setTickCount(3);
        axisXItem->setLabelFormat("%.2f");
        axisXItem->setTitleText(headersOnly[i]);
        axisXItem->setLabelsBrush(brushVector.at(brushVectorIndex));
        axisXItem->setTitleBrush(brushVector.at(brushVectorIndex));
        axisXs[i] = axisXItem;
    }
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
}

/*void MainWindow::on_series_Clicked(QPointF *point)
{
    if (series[currentRow]->pen().widthF() == 1)
    {
        series[currentRow]->pen().setWidthF(5);
    }
    else
    {
        series[currentRow]->pen().setWidthF(1);
    }

    ui->lblGisMethod->setText("line selected!");
}*/
void MainWindow::on_series_Clicked()
{
    auto serie = qobject_cast<QLineSeries*>(sender());
    QPen p = serie->pen();
    p.setWidth( p.width() == 1 ? 2: 1);
    serie->setPen(p);
    ui->listWidget->setCurrentRow(series.indexOf(serie));
    MainWindow::on_listWidget_itemClicked(ui->listWidget->currentItem());
}
