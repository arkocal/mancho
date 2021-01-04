#include <iostream>
#include <math.h>

#include <QThread>
#include <QtConcurrent>

#include "mainwindow.h"
#include "markermanager.h"
#include "ui_mainwindow.h"
#include "AudioFile.h"

// TODO kill threads on quit signal!

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->plot->addGraph();
    connect(ui->plot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(onMouseMove(QMouseEvent*)));
    connect(ui->plot, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(onMouseRelease(QMouseEvent*)));
    connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(onViewRangeChange(QCPRange)));
    connect(ui->plot, SIGNAL(mouseDoubleClick(QMouseEvent*)), this, SLOT(onPlotDoubleClick(QMouseEvent*)));
    isDragging = false;
    markerManager.setPlot(ui->plot);

    // Optics
    ui->plot->setBackground(QColor(230, 230, 230));
    ui->plot->axisRect()->setBackground(Qt::white);
    ui->plot->axisRect()->setAutoMargins(QCP::msNone);
    ui->plot->axisRect()->setMargins(QMargins(0, 0, 0, 20));
    ui->plot->xAxis->setVisible(true);
    ui->plot->yAxis->setVisible(false);
    ui->plot->xAxis->grid()->setVisible(true);
    ui->plot->yAxis->grid()->setVisible(false);
    ui->plot->setFocus(Qt::OtherFocusReason);
    // Marker line
    playheadLine = new QCPItemStraightLine(ui->plot);
    playheadLine->setPen(QPen(QColor(255, 0, 0), 2));
    // Interactions
    ui->plot->setInteraction(QCP::iRangeZoom, true);
    ui->plot->setInteraction(QCP::iRangeDrag, true);
    ui->plot->axisRect()->setRangeZoomAxes(ui->plot->xAxis, 0);
    ui->plot->axisRect()->setRangeDragAxes(ui->plot->xAxis, 0);
    timerId = startTimer(20);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadAudioFile(std::string path)
{
    this->setCursor(Qt::WaitCursor);
    playerConfig->mtx.lock();
    playerConfig->playFlag = 0;
    playerConfig->playheadTime = 0;
    playheadLine->point1->setCoords(0, 0);
    playheadLine->point2->setCoords(0, 1);
    // Load audio file
    playerConfig->audioFile.load(path);
    int numSamples = playerConfig->audioFile.getNumSamplesPerChannel();

    playerConfig->loopStartTime = 0;
    playerConfig->loopEndTime = float(numSamples) /
            float(playerConfig->audioFile.getSampleRate());
    playerConfig->hasFile = true;
    playerConfig->audioFile.printSummary();
    // TODO this should change based on sampling rate
    int downsampleBy = 25;

    QVector<double> x(numSamples/downsampleBy+1),
            y(numSamples/downsampleBy+1);
    for (int i = 0; i < numSamples/downsampleBy+1; i++)
    {
        double currentSample = playerConfig->audioFile.samples[0][i*downsampleBy];
        x[i] = float(i*downsampleBy)/playerConfig->audioFile.getSampleRate();
        y[i] = currentSample;
    }
    playerConfig->mtx.unlock();
    ui->plot->xAxis->setRange(0, numSamples/ playerConfig->audioFile.getSampleRate());
    ui->plot->yAxis->setRange(-1, 1);
    ui->plot->graph(0)->setData(x, y);
    ui->plot->replot();
    this->unsetCursor();
}

void MainWindow::togglePlay()
{
    if (!playerConfig->hasFile) // Can only toggle play if a file is loaded
        return;
    playerConfig->playFlag = !playerConfig->playFlag;
    playerConfig->playbackSpeed = float(ui->playbackSpeedSlider->value())/100;
    playerConfig->shouldSeek = true;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Space:
        togglePlay();
        break;
    case Qt::Key_A:
        markerManager.addTimeMarker(playerConfig->playheadTime);
        break;
    case Qt::Key_S:
        playerConfig->mtx.lock();
        selectAround(playerConfig->playheadTime);
        playerConfig->mtx.unlock();
        break;
    case Qt::Key_Backspace:
        markerManager.deleteFirstBefore(playerConfig->playheadTime);
        break;
    case Qt::Key_Delete:
        markerManager.deleteFirstAfter(playerConfig->playheadTime);
        break;
    }
}

void MainWindow::selectAround(float time)
{
    float startTime = markerManager.timeFirstBefore(time);
    float endTime = markerManager.timeFirstAfter(time);
    if (startTime == -1)
        startTime = 0;
    if (endTime == -1)
        endTime = playerConfig->endTime();
    //playerConfig->playFlag = 0;
    playerConfig->loopStartTime = startTime;
    playerConfig->loopEndTime = endTime;
    //playerConfig->playFlag = 1;

    if (selectionRect)
    {
        ui->plot->removeItem(selectionRect);
    }
    selectionRect = new QCPItemRect(ui->plot);
    selectionRect->setPen(QPen(QColor(255, 0, 0, 200)));
    selectionRect->setBrush(QBrush(QColor(200, 0, 0, 100)));
    selectionRect->topLeft->setCoords(startTime, 1);
    selectionRect->bottomRight->setCoords(endTime, -1);
    ui->plot->replot();
}


void MainWindow::onMouseMove(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        isDragging = true;
    }
}

void MainWindow::onMouseRelease(QMouseEvent *event)
{
    if (isDragging)
    {
        isDragging = false;
    }
    else
    {
        double x_co = ui->plot->xAxis->pixelToCoord(event->pos().x());
        if ( event->button() == Qt::LeftButton )
        {
            playerConfig->mtx.lock();
            selectAround(x_co);
            playerConfig->setTime(x_co);
            playerConfig->mtx.unlock();
        }
    }
}

void MainWindow::onPlotDoubleClick(QMouseEvent *event)
{
    double x_co = ui->plot->xAxis->pixelToCoord(event->pos().x());
    if ( event->button() == Qt::LeftButton )
    {
        markerManager.addTimeMarker(x_co);
    }
    // We may have to call the parent's method for other cases
    QMainWindow::mouseDoubleClickEvent(event);
}

void MainWindow::onViewRangeChange(const QCPRange &newRange)
{
    QCPRange boundedRange = newRange;
    double lowerRangeBound = 0;
    double size = boundedRange.size();
    if (size > playerConfig->endTime())
    {
        size = playerConfig->endTime();
    }
    double lower, upper;
    if(boundedRange.lower < lowerRangeBound)
    {
        lower = lowerRangeBound;
        upper = lowerRangeBound + size;
    }
    if(boundedRange.upper > playerConfig->endTime())
    {
        upper = playerConfig->endTime();
        lower = upper - size;
    }
    boundedRange.lower = lower;
    boundedRange.upper = upper;
    ui->plot->xAxis->setRange(boundedRange);
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    QCPRange range = ui->plot->xAxis->range();
    if (playerConfig->playFlag && (playerConfig->playheadTime > range.upper || playerConfig->playheadTime < range.lower))
    {
        auto size = range.size();
        range.lower = playerConfig->playheadTime;
        range.upper = range.lower + size;
        ui->plot->xAxis->setRange(range);
    }
    playheadLine->point1->setCoords(playerConfig->playheadTime, 0);
    playheadLine->point2->setCoords(playerConfig->playheadTime, 1);
    ui->plot->replot();
}

void MainWindow::on_updateTextButton_clicked()
{
    togglePlay();
}

void MainWindow::on_actionOpen_triggered()
{
    auto fileName = QFileDialog::getOpenFileName(this,
        tr("Open file"), QDir::homePath(), tr("Audio Files (*.wav)")).toStdString();
    loadAudioFile(fileName);
    std::cout << "Opened file" << fileName << std::endl;
}

void MainWindow::on_actionShow_Hide_Help_toggled(bool checked)
{
  ui->helpText->setVisible(checked);
}
