#ifndef QTWIDGETSAPPLICATIONFINAL_H
#define QTWIDGETSAPPLICATIONFINAL_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineF>
#include <QPointF>

enum InputMode {
    MODE_SEGMENT,
    MODE_POINT_QUERY,
    MODE_BAN
};

class QtWidgetsApplicationFinal : public QMainWindow {
    Q_OBJECT

public:
    QtWidgetsApplicationFinal(QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;

private slots:
    void clearScene();
    void buildTree();
    void prevVersion();
    void nextVersion();

private:
    QWidget* centralWidget;
    QGraphicsView* drawingView;
    QGraphicsView* treeView;
    QGraphicsView* treeView2;
    QGraphicsView* treeView3;
    QGraphicsScene* drawingScene;
    QGraphicsScene* treeScene;
    QGraphicsScene* treeScene2;
    QGraphicsScene* treeScene3;
    QLayout* treeLayout;

    QPushButton* btnClear;
    QPushButton* btnBuildTree;
    QPushButton* btnPrevVersion;
    QPushButton* btnNextVersion;

    QPushButton* nextButton;
    QPushButton* continueButton;
    QPushButton* pauseButton;

    QVector<QPointF> clickPoints;
    QVector<QLineF> segments;
    int versionIndex;
    InputMode currentMode;
    int tot;

    QTabWidget* tabWidget;
    
    QGraphicsScene* treeScene4;

    void drawSegments();
    void drawTree();
    void highlightNearestAbove(QPointF );
    QPointF mapToSceneCoords(const QPoint&);
    void showSplit(int, int, double);
    int find_id(QPointF point);
    bool isPaused = true;      // 是否暂停
    bool isStepMode = false;   // 是否单步调试

    //void drawTreeNode(T* node, int depth, int& xPos, QGraphicsScene* scene, int parentX, int parentY);
    //void DrawTree(T* Rt);

};



class TreapVisualizer : public QWidget {
    Q_OBJECT

public:
    TreapVisualizer(QWidget* parent ,int id);
protected:
    //void paintEvent(QPaintEvent* event) override;
};

#endif // QTWIDGETSAPPLICATIONFINAL_H
