#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <vector>
#include <string>
#include <QMainWindow>
#include <QAudioFormat>
#include <QThread>

#include "markermanager.h"
#include "qcustomplot.h"
#include "playerconfig.h"
#include "AudioFile.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static QThread *audioThread;
    static PlayerConfig *playerConfig;
private slots:
    void on_updateTextButton_clicked();
    void onMouseMove(QMouseEvent *event);
    void onMouseRelease(QMouseEvent *event);
    void onPlotDoubleClick(QMouseEvent *event);
    void on_actionOpen_triggered();
    void keyPressEvent(QKeyEvent *event);
    void onViewRangeChange(const QCPRange&);
    void on_actionShow_Hide_Help_toggled(bool arg1);

private:
    Ui::MainWindow *ui;
    int timerId;
    QCPItemRect *selectionRect;
    QCPItemStraightLine *playheadLine;
    void loadAudioFile(std::string path);
    void togglePlay();
    void selectAround(float time);
    MarkerManager markerManager;
    bool isDragging;
protected:
    void timerEvent(QTimerEvent *event);
};
#endif // MAINWINDOW_H
