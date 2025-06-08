#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QPushButton;
class DrawingCanvas;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void onConvexConvexClicked();
    void onConvexNonconvexClicked();

private:
    QPushButton *btnConvexConvex;
    QPushButton *btnConvexNonconvex;
    QPushButton *btnInputPolygon;
    QPushButton *btnTriangulate;
    QPushButton *btnSumEach;
    QPushButton *btnMerge;
    QPushButton *btnReset;
    DrawingCanvas *canvas;
};

#endif // MAINWINDOW_H
