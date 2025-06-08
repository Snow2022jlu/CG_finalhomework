#ifndef UTILS_H
#define UTILS_H

#include <QVector>
#include <QPointF>

QVector<QPointF> convex_plus_convex(const QVector<QPointF> &a, const QVector<QPointF> &b);

QVector<QVector<QPointF>> triangulate(const QVector<QPointF>& polygon);

QVector<QPointF> convex_hull(const QVector<QPointF>& pts);

double cross_three(const QPointF &a, const QPointF &b, const QPointF &c);


#endif // UTILS_H
