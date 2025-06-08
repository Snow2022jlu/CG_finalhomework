#include "mainwindow.h"
#include "utils.h"
#include "drawingcanvas.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget;
    setCentralWidget(central);
    setWindowTitle("Minkowski Sum Visualizer");

    btnConvexConvex = new QPushButton("凸 + 凸");
    btnConvexNonconvex = new QPushButton("凸 + 非凸");
    btnInputPolygon = new QPushButton("输入多边形");

    btnTriangulate = new QPushButton("三角剖分");
    btnSumEach = new QPushButton("分别求和");
    btnMerge = new QPushButton("合并");
    btnReset = new QPushButton("重置");

    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(btnConvexConvex);
    buttonLayout->addWidget(btnConvexNonconvex);
    buttonLayout->addWidget(btnInputPolygon);
    buttonLayout->addWidget(btnTriangulate);
    buttonLayout->addWidget(btnSumEach);
    buttonLayout->addWidget(btnMerge);
    buttonLayout->addWidget(btnReset);
    buttonLayout->addStretch();

    canvas = new DrawingCanvas;

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(canvas, 1);

    central->setLayout(mainLayout);

    connect(btnConvexConvex, &QPushButton::clicked, this, &MainWindow::onConvexConvexClicked);
    connect(btnConvexNonconvex, &QPushButton::clicked, this, &MainWindow::onConvexNonconvexClicked);
    connect(btnInputPolygon, &QPushButton::clicked, canvas, &DrawingCanvas::startPolygonInput);
    connect(btnTriangulate, &QPushButton::clicked, canvas, &DrawingCanvas::doTriangulation);
    connect(btnSumEach, &QPushButton::clicked, canvas, &DrawingCanvas::doPairwiseSum);
    connect(btnMerge, &QPushButton::clicked, canvas, &DrawingCanvas::doMerge);
    connect(btnReset, &QPushButton::clicked, canvas, &DrawingCanvas::reset);
}

void MainWindow::onConvexConvexClicked() {
    canvas->setMode(DrawingCanvas::ConvexConvex);
    canvas->computeMinkowski();
}

void MainWindow::onConvexNonconvexClicked() {
    canvas->setMode(DrawingCanvas::ConvexNonconvex);
}