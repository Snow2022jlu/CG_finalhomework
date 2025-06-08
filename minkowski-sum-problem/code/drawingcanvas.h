#ifndef DRAWINGCANVAS_H
#define DRAWINGCANVAS_H

#include <QWidget>
#include <QVector>
#include <QPointF>

class DrawingCanvas : public QWidget {
    Q_OBJECT

public:
    enum Mode {
        ConvexConvex,
        ConvexNonconvex
    };

    explicit DrawingCanvas(QWidget *parent = nullptr);

    void setMode(Mode mode);
    void startPolygonInput();
    void computeMinkowski();

    QPainterPath mergeRecursive(const QList<QPolygonF>& polys);

    void doTriangulation();
    void doPairwiseSum();
    void doMerge();

    void reset();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    enum InputState {
        WaitingFirst,
        WaitingSecond,
        ReadyToCompute
    };

    enum MergeState {
        step1,
        step2
    };

    Mode currentMode;
    InputState state;
    MergeState mergeState;

    QVector<QPointF> poly1;
    QVector<QPointF> poly2;
    QVector<QPointF> currentChain;
    QVector<QPointF> resultPolygon;
    QVector<QVector<QPointF>> triangleList;
    QVector<QVector<QPointF>> partialResults;
    QVector<QPolygonF> holesPolygons;


    bool inputtingPolygon;

    void drawGrid(QPainter &painter);
    void drawPolygons(QPainter &painter);
};

#endif // DRAWINGCANVAS_H
