#ifndef MARKERMANAGER_H
#define MARKERMANAGER_H

#include <vector>
#include "qcustomplot.h"

class MarkerManager
{
public:
    MarkerManager();
    void addTimeMarker(float time);
    void setPlot(QCustomPlot* p);
    void deleteFirstBefore(float time);
    void deleteFirstAfter(float time);
    float timeFirstBefore(float time);
    float timeFirstAfter(float time);
private:
    QCustomPlot* plot;
    std::vector<QCPItemStraightLine*> markers;
    int firstIndBefore(float time);
    int firstIndAfter(float time);
};

#endif // MARKERMANAGER_H
