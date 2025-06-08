#include "drawingcanvas.h"
#include "utils.h"

#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QPainterPath>
#include <QtMath>

// 常量
static constexpr double SCALE = 50.0;

// 坐标变换：世界 → 屏幕
static QPointF worldToScreen(const QPointF &pt, const QPointF &origin) {
    return QPointF(origin.x() + pt.x() * SCALE,
                   origin.y() - pt.y() * SCALE);
}

// 坐标变换：屏幕 → 世界
static QPointF screenToWorld(const QPointF &pt, const QPointF &origin) {
    return QPointF((pt.x() - origin.x()) / SCALE,
                   -(pt.y() - origin.y()) / SCALE);
}

DrawingCanvas::DrawingCanvas(QWidget *parent)
    : QWidget(parent), currentMode(ConvexConvex),
      state(WaitingFirst), inputtingPolygon(false), mergeState(step1)
{
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(600, 400);
}

void DrawingCanvas::setMode(Mode mode) {
    currentMode = mode;
}

void DrawingCanvas::startPolygonInput() {
    if (state == ReadyToCompute) {
        poly1.clear();
        poly2.clear();
        resultPolygon.clear();
        state = WaitingFirst;
    }
    currentChain.clear();
    inputtingPolygon = true;
    update();
}

void DrawingCanvas::mousePressEvent(QMouseEvent *event) {
    if (inputtingPolygon && event->button() == Qt::LeftButton) {
        QPointF origin(width() / 2, height() / 2);
        QPointF worldPt = screenToWorld(event->pos(), origin);
        currentChain.push_back(worldPt);
        update();
    }
}

void DrawingCanvas::keyPressEvent(QKeyEvent *event) {
    if (inputtingPolygon && event->key() == Qt::Key_Return) {
        if (currentChain.size() >= 3) {
            if (currentChain.first() != currentChain.last())
                currentChain.push_back(currentChain.first());

            if (state == WaitingFirst) {
                poly1 = currentChain;
                state = WaitingSecond;
            } else if (state == WaitingSecond) {
                poly2 = currentChain;
                state = ReadyToCompute;
            }
        }
        currentChain.clear();
        inputtingPolygon = false;
        update();
    }
}

void DrawingCanvas::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawGrid(painter);
    drawPolygons(painter);
}

void DrawingCanvas::drawGrid(QPainter &painter) {
    QPointF origin(width() / 2, height() / 2);
    int w = width(), h = height();

    QPen gridPen(QColor(230, 230, 230));
    painter.setPen(gridPen);

    for (int i = -100; i <= 100; ++i) {
        int x = origin.x() + i * SCALE;
        painter.drawLine(x, 0, x, h);
    }

    for (int i = -100; i <= 100; ++i) {
        int y = origin.y() + i * SCALE;
        painter.drawLine(0, y, w, y);
    }

    QPen axisPen(Qt::gray);
    axisPen.setWidth(2);
    painter.setPen(axisPen);
    painter.drawLine(0, origin.y(), w, origin.y());
    painter.drawLine(origin.x(), 0, origin.x(), h);
}

void DrawingCanvas::drawPolygons(QPainter &painter) {
    QPointF origin(width() / 2, height() / 2);

    auto drawChain = [&](const QVector<QPointF> &chain, QColor color, bool dashed = false) {
        if (chain.isEmpty()) return;

        QPen pen(color);
        pen.setWidth(2);
        if (dashed) pen.setStyle(Qt::DashLine);
        painter.setPen(pen);
        painter.setBrush(color);

        QVector<QPointF> mapped;
        for (const auto &pt : chain)
            mapped.push_back(worldToScreen(pt, origin));

        for (int i = 0; i + 1 < mapped.size(); ++i)
            painter.drawLine(mapped[i], mapped[i + 1]);

        for (const QPointF &pt : mapped)
            painter.drawEllipse(pt, 3, 3);
    };

    drawChain(poly1, Qt::blue);
    drawChain(poly2, Qt::darkCyan);
    if (inputtingPolygon)
        drawChain(currentChain, Qt::red, true);
    drawChain(resultPolygon, Qt::darkGreen);

    if (triangleList.size() > 0) {
        QPointF origin(width() / 2, height() / 2);
        QPen triPen(Qt::darkRed);
        triPen.setWidth(1);
        painter.setPen(triPen);

        QBrush triBrush(QColor(255, 0, 0, 30));
        painter.setBrush(triBrush);

        for (const auto& tri : triangleList) {
            if (tri.size() < 3) continue;

            QVector<QPointF> screenTri;
            for (const auto& pt : tri)
                screenTri.append(worldToScreen(pt, origin));

            QPolygonF poly(screenTri);
            painter.drawPolygon(poly);
        }
    }

    if (!partialResults.isEmpty()) {
        QPointF origin(width() / 2, height() / 2);

        for (int i = 0; i < partialResults.size(); ++i) {
            const auto& res = partialResults[i];
            QVector<QPointF> screenRes;
            for (const auto& pt : res)
                screenRes.append(worldToScreen(pt, origin));

            // 生成不同颜色（例如用 HSV 色环）
            QColor fillColor = QColor::fromHsv((i * 40) % 360, 160, 255, 80);  // 色相变化
            QColor borderColor = fillColor.darker(150);

            QPen pen(borderColor);
            pen.setWidth(2);
            QBrush brush(fillColor);

            painter.setPen(pen);
            painter.setBrush(brush);
            painter.drawPolygon(QPolygonF(screenRes));
        }
    }

    if (!holesPolygons.isEmpty()) {
        QPointF origin(width() / 2, height() / 2);
        QPen holePen(Qt::darkGreen);
        holePen.setWidth(2);
        painter.setPen(holePen);
        painter.setBrush(Qt::NoBrush);  // ❗️确保取消填充颜色

        for (const auto& hole : holesPolygons) {
            QVector<QPointF> screenHole;
            for (const auto& pt : hole)
                screenHole.append(worldToScreen(pt, origin));

            // 画边框（空心）
            painter.drawPolygon(QPolygonF(screenHole));

            // 顶点（用红点标出）
            QBrush pointBrush(Qt::darkGreen);
            painter.setBrush(pointBrush);
            for (const QPointF& pt : screenHole)
                painter.drawEllipse(pt, 3, 3);
            
            painter.setBrush(Qt::NoBrush);  // ❗️记得恢复为空，以免点影响后续轮廓
        }
    }
}

void DrawingCanvas::computeMinkowski() {
    if (state != ReadyToCompute) return;

    resultPolygon.clear();

    if (currentMode == ConvexConvex) {
        resultPolygon = convex_plus_convex(poly1, poly2);
    }

    update();
}

void DrawingCanvas::doTriangulation() {
    triangleList.clear();

    triangleList = triangulate(poly2);  // 只有 poly2 是非凸

    update();
}

void DrawingCanvas::doPairwiseSum() {
    partialResults.clear();
    for (const auto& tri : triangleList) {
        partialResults.push_back(convex_plus_convex(poly1, tri));
    }

    triangleList.clear();

    update();
}



static double polygonArea(const QPolygonF &poly) {
    double area = 0;
    for (int i = 0; i < poly.size(); ++i) {
        const QPointF &p1 = poly[i];
        const QPointF &p2 = poly[(i + 1) % poly.size()];
        area += (p1.x() * p2.y() - p2.x() * p1.y());
    }
    return qAbs(area / 2.0);
}


QPainterPath DrawingCanvas::mergeRecursive(const QList<QPolygonF>& polys) {
    if (polys.isEmpty())
        return QPainterPath();
    if (polys.size() == 1) {
        QPainterPath path;
        path.addPolygon(polys[0]);
        return path;
    }

    int mid = polys.size() / 2;
    QList<QPolygonF> left = polys.mid(0, mid);
    QList<QPolygonF> right = polys.mid(mid);

    QPainterPath pathL = mergeRecursive(left);
    QPainterPath pathR = mergeRecursive(right);

    return pathL.united(pathR);  // Qt 自动处理空洞合并
}


void DrawingCanvas::doMerge() {
    if (partialResults.isEmpty()) return;

    // 构造初始 QPolygonF 队列
    QList<QPolygonF> polyList;
    for (const auto& poly : partialResults) {
        if (poly.size() >= 3)
            polyList.append(QPolygonF(poly));
    }
    if (polyList.isEmpty()) return;

    // 二路归并：最终合并为一个 QPainterPath
    QPainterPath finalPath = mergeRecursive(polyList);

    // 提取所有子路径（主轮廓 + 空洞）
    QList<QPolygonF> allPolys = finalPath.toSubpathPolygons();
    if (allPolys.isEmpty()) return;

    // 找出最大面积的路径作为外轮廓，其余为空洞
    int maxIdx = 0;
    double maxArea = polygonArea(allPolys[0]);
    for (int i = 1; i < allPolys.size(); ++i) {
        double area = polygonArea(allPolys[i]);
        if (area > maxArea) {
            maxArea = area;
            maxIdx = i;
        }
    }

    resultPolygon = allPolys[maxIdx];
    holesPolygons.clear();
    for (int i = 0; i < allPolys.size(); ++i) {
        if (i != maxIdx)
            holesPolygons.append(allPolys[i]);
    }

    if (step1 == mergeState) {
        mergeState = step2;
    } else if (step2 == mergeState) {
        partialResults.clear();
    }

    update();
}



void DrawingCanvas::reset() {
    poly1.clear();
    poly2.clear();
    currentChain.clear();
    resultPolygon.clear();
    triangleList.clear();
    partialResults.clear();
    holesPolygons.clear();
    state = WaitingFirst;
    mergeState = step1;
    inputtingPolygon = false;
    update();
}
