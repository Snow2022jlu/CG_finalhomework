#include "utils.h"
#include <QVector>
#include <QPointF>
#include <algorithm>
#include <QDebug>

static double cross(const QPointF &a, const QPointF &b) {
    return a.x() * b.y() - a.y() * b.x();
}

static int leftmost_index(const QVector<QPointF> &poly) {
    int idx = 0;
    for (int i = 1; i < poly.size(); ++i) {
        if (poly[i].x() < poly[idx].x() ||
           (poly[i].x() == poly[idx].x() && poly[i].y() < poly[idx].y())) {
            idx = i;
        }
    }
    return idx;
}

static QVector<QPointF> edge_vectors(const QVector<QPointF> &poly) {
    QVector<QPointF> vecs;
    for (int i = 0; i < poly.size(); ++i) {
        vecs.push_back(poly[(i + 1) % poly.size()] - poly[i]);
    }
    return vecs;
}

QVector<QPointF> convex_plus_convex(const QVector<QPointF> &a_in, const QVector<QPointF> &b_in) {
    QVector<QPointF> a = a_in, b = b_in;
    if (a.first() == a.last()) a.pop_back();
    if (b.first() == b.last()) b.pop_back();

    int na = a.size(), nb = b.size();
    if (na < 3 || nb < 3) return {};

    // 旋转使左下角在首位
    std::rotate(a.begin(), a.begin() + leftmost_index(a), a.end());
    std::rotate(b.begin(), b.begin() + leftmost_index(b), b.end());

    QVector<QPointF> ea = edge_vectors(a);
    QVector<QPointF> eb = edge_vectors(b);

    QVector<QPointF> result;
    QPointF cur = a[0] + b[0];
    result.push_back(cur);

    int ia = 0, ib = 0;
    while (ia < ea.size() || ib < eb.size()) {
        QPointF va = (ia < ea.size()) ? ea[ia] : QPointF(0, 0);
        QPointF vb = (ib < eb.size()) ? eb[ib] : QPointF(0, 0);

        double c = cross(va, vb);
        if (ib >= eb.size() || (ia < ea.size() && c >= 0)) {
            cur += va;
            ++ia;
        } else {
            cur += vb;
            ++ib;
        }
        result.push_back(cur);
    }

    // 闭合
    if (result.first() != result.last())
        result.push_back(result.first());

    return result;
}

static QPointF subtract(const QPointF& a, const QPointF& b) {
    return QPointF(a.x() - b.x(), a.y() - b.y());
}

static double polygonSignedArea(const QVector<QPointF>& poly) {
    double area = 0;
    int n = poly.size();
    for (int i = 0; i < n; ++i) {
        const QPointF& p1 = poly[i];
        const QPointF& p2 = poly[(i + 1) % n];
        area += (p1.x() * p2.y() - p2.x() * p1.y());
    }
    return area / 2.0;
}

static bool pointInTriangle(const QPointF& p, const QPointF& a, const QPointF& b, const QPointF& c, bool ccw) {
    double c1 = cross(subtract(b, a), subtract(p, a));
    double c2 = cross(subtract(c, b), subtract(p, b));
    double c3 = cross(subtract(a, c), subtract(p, c));

    if (ccw)
        return (c1 > 1e-8 && c2 > 1e-8 && c3 > 1e-8);
    else
        return (c1 < -1e-8 && c2 < -1e-8 && c3 < -1e-8);
}

// 主体函数
QVector<QVector<QPointF>> triangulate(const QVector<QPointF>& polygon) {
    QVector<QVector<QPointF>> result;
    int n = polygon.size();
    if (n < 3) return result;

    QVector<int> indices(n);
    for (int i = 0; i < n; ++i) indices[i] = i;

    bool ccw = polygonSignedArea(polygon) > 0;
    fprintf(stderr, "Polygon is %s\n", ccw ? "CCW" : "CW");

    int remaining = n;
    int loop_guard = 0;

    while (remaining > 3 && loop_guard++ < 500) {
        bool earFound = false;

        for (int i = 0; i < remaining; ++i) {
            int i0 = indices[(i + remaining - 1) % remaining];
            int i1 = indices[i];
            int i2 = indices[(i + 1) % remaining];

            const QPointF& a = polygon[i0];
            const QPointF& b = polygon[i1];
            const QPointF& c = polygon[i2];

            QPointF ab = subtract(b, a);
            QPointF bc = subtract(c, b);
            double cp = cross(ab, bc);

            // 修正后的凸角判断
            if ((ccw && cp < 1e-8) || (!ccw && cp > -1e-8)) continue;

            // 检查三角形内部是否有其他点
            bool hasInner = false;
            for (int j = 0; j < remaining; ++j) {
                int idx = indices[j];
                if (idx == i0 || idx == i1 || idx == i2) continue;
                if (pointInTriangle(polygon[idx], a, b, c, ccw)) {
                    hasInner = true;
                    break;
                }
            }

            if (!hasInner) {
                result.append({a, b, c});
                indices.removeAt(i);
                --remaining;
                earFound = true;
                break;
            }
        }

        if (!earFound) {
            fprintf(stderr, "No ear found, triangulation failed.\n");
            break;
        }
    }

    if (remaining == 3) {
        result.append({
            polygon[indices[0]],
            polygon[indices[1]],
            polygon[indices[2]]
        });
    }

    fprintf(stderr, "Triangulation completed with %d triangles.\n", result.size());

    return result;
}

double cross_three(const QPointF &a, const QPointF &b, const QPointF &c) {
    return (b.x() - a.x()) * (c.y() - a.y()) - (b.y() - a.y()) * (c.x() - a.x());
}


QVector<QPointF> convex_hull(const QVector<QPointF>& pts_in) {
    QVector<QPointF> pts = pts_in;
    std::sort(pts.begin(), pts.end(), [](const QPointF &a, const QPointF &b) {
        return a.x() == b.x() ? a.y() < b.y() : a.x() < b.x();
    });

    QVector<QPointF> lower, upper;
    for (const auto& p : pts) {
        while (lower.size() >= 2 && cross_three(lower[lower.size()-2], lower[lower.size()-1], p) <= 0)
            lower.pop_back();
        lower.push_back(p);
    }

    for (int i = pts.size() - 1; i >= 0; --i) {
        const auto& p = pts[i];
        while (upper.size() >= 2 && cross_three(upper[upper.size()-2], upper[upper.size()-1], p) <= 0)
            upper.pop_back();
        upper.push_back(p);
    }

    lower.pop_back();
    upper.pop_back();
    lower += upper;
    return lower;
}