#include <iostream>
#include "markermanager.h"
#include "qcustomplot.h"

MarkerManager::MarkerManager()
{
}

void MarkerManager::setPlot(QCustomPlot* plot)
{
    this->plot = plot;
}

void MarkerManager::addTimeMarker(float time)
{
    QCPItemStraightLine *newMarker = new QCPItemStraightLine(plot);
    newMarker->point1->setCoords(time, 0);  // location of point 1 in plot coordinate
    newMarker->point2->setCoords(time, 1);  // location of point 2 in plot coordinate
    markers.push_back(newMarker);
    plot->replot();
}

int MarkerManager::firstIndBefore(float time)
{
    if (markers.empty())
        return -1;
    int result = -1;
    for(int i=0; i<markers.size(); i++)
    {
        QCPItemStraightLine* line = markers[i];
        if (result < 0)
        {
            if (line->point1->key() <= time)
            {
                result = i;
            }
        }
        else if (line->point1->key() >= markers[result]->point1->key() &&
                 line->point1->key() <= time)
        {
            result = i;
        }
    }
    return result;
}

int MarkerManager::firstIndAfter(float time)
{
    if (markers.empty())
        return -1;
    int result = -1;
    for(int i=0; i<markers.size(); i++)
    {
        QCPItemStraightLine* line = markers[i];
        if (result < 0)
        {
            if (line->point1->key() > time)
            {
                result = i;
            }
        }
        else if (line->point1->key() <= markers[result]->point1->key() &&
                 line->point1->key() > time)
        {
            result = i;
        }
    }
    return result;
}

void MarkerManager::deleteFirstBefore(float time)
{
    int index = firstIndBefore(time);
    if (index < 0)
    {
        return;
    }
    QCPItemStraightLine* line = markers[index];
    plot->removeItem(line);
    markers.erase(markers.begin() + index);
}

void MarkerManager::deleteFirstAfter(float time)
{
    int index = firstIndAfter(time);
    if (index < 0)
    {
        return;
    }
    QCPItemStraightLine* line = markers[index];
    plot->removeItem(line);
    markers.erase(markers.begin() + index);
}

float MarkerManager::timeFirstBefore(float time)
{
    int index = firstIndBefore(time);
    if (index==-1) return -1;
    else
        return (markers[index]->point1->key());
}

float MarkerManager::timeFirstAfter(float time)
{
    int index = firstIndAfter(time);
    if (index==-1) return -1;
    else
        return (markers[index]->point1->key());
}
