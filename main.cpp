#include "mainwindow.h"
#include "playerconfig.h"
#include "audioloop.h"
#include <QApplication>
#include <QThread>


PlayerConfig *MainWindow::playerConfig = new PlayerConfig();
QThread *MainWindow::audioThread = QThread::create([]{ audioLoop(); });

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w; 
    w.show();
    // TODO join this thread
    MainWindow::audioThread->start();
    return a.exec();
}
