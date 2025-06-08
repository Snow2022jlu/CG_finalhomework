#include "QtWidgetsApplicationFinal.h"
#include <QApplication>
#include <QThread.h>
#include <QMouseEvent>
#include <QGraphicsLineItem>
#include <QLabel>
#include <QMessagebox>
#include <queue>
#include <iostream>
#include <QTimer>

inline int Rand() {
    static int x = 23333;
    return x ^= x << 13, x ^= x >> 17, x ^= x << 5;
}

struct Event {
    int id, type;
    double time;
    Event() {}
    Event(double _time, int _id, int _type) :time(_time), id(_id), type(_type) {}
    bool operator < (const Event& other) const {
        if (time != other.time) return time > other.time;
        if (type != other.type) return type < other.type;
        if (id != other.id) return id > other.id;
        return false; // 保证相同的事件不会被重复处理
    }
};

Event Tsg;

struct T {
    T* s[2]; int key, siz, val;
    T() {}
    T(int x) { s[0] = s[1] = NULL; key = x; siz = 1; val = Rand(); }
    void rub() {
        siz = 1;
        if (s[0]) siz += s[0]->siz;
        if (s[1]) siz += s[1]->siz;
    }
};

#define N 1000007
double x1[N], Y1[N], x2[N], y2[N];

#define eps 1e-9
#define Eps 1e-6
#define EPS 1e-3
#define Ma (INT_MAX>>1)

void Spilt(T* now, int k, T*& x, T*& y) {
    if (!now) { x = y = NULL; return; }
    int cmp = now->s[0] ? now->s[0]->siz + 1 : 1;
    if (k < cmp) { y = new T(); *y = *now; Spilt(y->s[0], k, x, y->s[0]); y->rub(); }
    else { x = new T(); *x = *now; Spilt(x->s[1], k-cmp, x->s[1], y); x->rub(); }
}
bool leq(int id1, int id2, double time) {
    if (id1 == Ma) return false;
    if (id2 == Ma) return true;
    double YY1 = Y1[id1] + (time - x1[id1]) * (y2[id1] - Y1[id1]) / (x2[id1] - x1[id1]);
    double YY2 = Y1[id2] + (time - x1[id2]) * (y2[id2] - Y1[id2]) / (x2[id2] - x1[id2]);
    return YY1 < YY2 - eps || (fabs(YY1 - YY2) < eps && (y2[id1] - Y1[id1]) / (x2[id1] - x1[id1]) <= (y2[id2] - Y1[id2]) / (x2[id2] - x1[id2]));
}
void Spilt2(T* now, int k, T*& x, T*& y, double time) {
    if (!now) { x = y = NULL; return; }
    if (leq(k, now->key, time)) { y = new T(); *y = *now; Spilt2(y->s[0], k, x, y->s[0], time); y->rub(); }
    else { x = new T(); *x = *now; Spilt2(x->s[1], k, x->s[1], y, time); x->rub(); }
}

T* Merge(T* x, T* y) {
    if (!x) return y; if (!y) return x;
    if (x->val < y->val) {
        T* g = new T(); *g = *x;
        g->s[1] = Merge(g->s[1], y); g->rub(); return g;
    }
    T* g = new T(); *g = *y;
    g->s[0] = Merge(x, g->s[0]); g->rub(); return g;
}

QtWidgetsApplicationFinal::QtWidgetsApplicationFinal(QWidget* parent)
    : QMainWindow(parent), versionIndex(0) {
    currentMode = MODE_SEGMENT;
    tot = 0;
    isPaused = true;      // 是否暂停
    isStepMode = false;   // 是否单步调试
    
    // === UI Setup ===
// === UI Setup ===
    centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    QVBoxLayout* windowLayout = new QVBoxLayout(centralWidget); // 主窗口布局
    tabWidget = new QTabWidget(this);
    windowLayout->addWidget(tabWidget);

    // ==== 第一个标签页 ====
    QWidget* page1 = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(page1);

    // drawing view
    drawingView = new QGraphicsView(this);
    drawingScene = new QGraphicsScene(this);
    drawingView->setScene(drawingScene);
    mainLayout->addWidget(drawingView);

    // 控制按钮布局
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    btnClear = new QPushButton("show spilt way");
    btnBuildTree = new QPushButton("build");
    nextButton = new QPushButton("next");
    continueButton = new QPushButton("continue");
    pauseButton = new QPushButton("pause");
    btnPrevVersion = new QPushButton("run");
    btnNextVersion = new QPushButton("run back");

    buttonLayout->addWidget(btnClear);
    buttonLayout->addWidget(btnBuildTree);
    buttonLayout->addWidget(nextButton);
    buttonLayout->addWidget(continueButton);
    buttonLayout->addWidget(pauseButton);
    mainLayout->addLayout(buttonLayout);

    // tree views
    treeLayout = new QHBoxLayout();
    treeView = new QGraphicsView();
    treeScene = new QGraphicsScene(this);
    treeView->setScene(treeScene);

    treeView2 = new QGraphicsView();
    treeScene2 = new QGraphicsScene(this);
    treeView2->setScene(treeScene2);

    treeView3 = new QGraphicsView();
    treeScene3 = new QGraphicsScene(this);
    treeView3->setScene(treeScene3);

    treeLayout->addWidget(treeView);
    treeLayout->addWidget(treeView2);
    treeLayout->addWidget(treeView3);
    mainLayout->addLayout(treeLayout);

    // 添加第一个标签页
    tabWidget->addTab(page1, "Main View");

    // ==== 第二个标签页 ====
    QWidget* page2 = new QWidget();
    QVBoxLayout* layout2 = new QVBoxLayout(page2);
    auto treeView4 = new QGraphicsView();
    treeScene4 = new QGraphicsScene(this);
    treeView4->setScene(treeScene4);
    layout2->addWidget(treeView4);
    tabWidget->addTab(page2, "Second View");

    // ==== 控件连接 ====
    connect(btnClear, &QPushButton::clicked, this, &QtWidgetsApplicationFinal::clearScene);
    connect(btnBuildTree, &QPushButton::clicked, this, &QtWidgetsApplicationFinal::buildTree);

    connect(nextButton, &QPushButton::clicked, this, [&]() {
        isPaused = false;
        isStepMode = true;
        });

    connect(continueButton, &QPushButton::clicked, this, [&]() {
        isPaused = false;
        isStepMode = false;
        });

    connect(pauseButton, &QPushButton::clicked, this, [&]() {
        isPaused = true;
        });

    drawingScene->setSceneRect(0, 0, 1600, 600);

    // 设置窗口
    this->setWindowTitle("Point Location by Persistent Treap - Qt C++");
    this->resize(1800, 1600);
}

void QtWidgetsApplicationFinal::mousePressEvent(QMouseEvent* event) {
    if (!drawingView->geometry().contains(event->pos())) return;

    QPointF scenePos = mapToSceneCoords(event->pos());

    if (currentMode == MODE_SEGMENT) {
        clickPoints.append(scenePos);
        if (clickPoints.size() == 2) {
            QLineF line(clickPoints[0], clickPoints[1]);
            segments.append(line);
            clickPoints.clear();
            drawSegments();
        }
    }
    else if (currentMode == MODE_POINT_QUERY) {
        highlightNearestAbove(scenePos);
    }
}

QPointF QtWidgetsApplicationFinal::mapToSceneCoords(const QPoint& point) {
    QPoint localPoint = point - drawingView->mapToParent(QPoint(0, 0));
    return drawingView->mapToScene(localPoint);
}

void QtWidgetsApplicationFinal::drawSegments() {
    drawingScene->clear();

    const int radius = 5;
    int index = 1;

    for (const QLineF& line : segments) {
        // 画线
        drawingScene->addLine(line, QPen(Qt::darkGray, 2));

        // 起点圆圈
        QRectF startCircle(line.p1().x() - radius, line.p1().y() - radius, radius * 2, radius * 2);
        QGraphicsEllipseItem* startDot = drawingScene->addEllipse(startCircle, QPen(Qt::black), QBrush(Qt::white));
        startDot->setToolTip(QString("(%1, %2)").arg(line.p1().x()).arg(line.p1().y()));

        // 终点圆圈
        QRectF endCircle(line.p2().x() - radius, line.p2().y() - radius, radius * 2, radius * 2);
        QGraphicsEllipseItem* endDot = drawingScene->addEllipse(endCircle, QPen(Qt::black), QBrush(Qt::white));
        //endDot->setToolTip(QString("(%.1f, %.1f)").arg(line.p2().x()).arg(line.p2().y()));
        endDot->setToolTip(QString("(%1, %2)").arg(line.p2().x()).arg(line.p2().y()));

        // 编号显示在中点
        QGraphicsTextItem* label = drawingScene->addText(QString::number(index));
        label->setDefaultTextColor(Qt::blue);
        label->setFont(QFont("Arial", 10, QFont::Bold));
        label->setPos((line.p1().x() + line.p2().x()) / 2, (line.p1().y() + line.p2().y()) / 2);

        ++index;
    }
}
QGraphicsLineItem* scanLine = nullptr;
// 在函数中执行这个动画
void animateScanLine(double x0, double x1, double step, QGraphicsScene* scene) {

    // 如果之前画过，先删除旧的扫描线
    if (scanLine) {
        scene->removeItem(scanLine);
        delete scanLine;
        scanLine = nullptr;
    }
    if (std::abs(x1 + 1) == 0) {
        scene->removeItem(scanLine);
        delete scanLine;
        scanLine = nullptr;
        return;
    }
    int sceneHeight = 1600;
    std::cout << "scene height:" << sceneHeight;

    for (double x = x0; x <= x1; x += step) {
        // 清除旧线
        if (scanLine) {
            scene->removeItem(scanLine);
            delete scanLine;
            scanLine = nullptr;
        }
        
        scanLine = scene->addLine(x, -200, x, sceneHeight, QPen(Qt::red, 2));
        // 强制界面刷新
        QApplication::processEvents();

        // 可调节速度
        QThread::msleep(10);
    }
}

void drawTreeNode(T* node, int depth, double xPos, double step,QGraphicsScene* scene, double parentX, double parentY) {
    if (!node) return;

    const int r = 15; // 圆半径
    const int hGap = 60; // 水平间距
    const int vGap = 40; // 垂直间距

    
    // 当前节点位置
    int x = xPos * hGap + 40;
    int y = depth * vGap + 10;
    // 先画左子树，累加水平位置
    

    // 画连线（父 → 当前）
    if (parentX != -1 && parentY != -1) {
        scene->addLine(parentX, parentY, x, y, QPen(Qt::black));
    }

    // 画圆圈
    auto* circle = scene->addEllipse(x - r, y - r, 2 * r, 2 * r, QPen(Qt::black), QBrush(Qt::white));
    circle->setZValue(-10);


    // 写 key
    QGraphicsTextItem* text = scene->addText(QString::number(node->key));
    text->setPos(x - 10, y - 12);

    // 设置悬停提示（val 和 siz）
    QString tooltip = QString("val = %1\nsiz = %2").arg(node->val).arg(node->siz);
    circle->setToolTip(tooltip);
    text->setToolTip(tooltip);

    drawTreeNode(node->s[0], depth + 1, xPos - step, step * 0.5, scene, x, y);
    // 画右子树
    drawTreeNode(node->s[1], depth + 1, xPos + step, step * 0.5, scene, x, y);
}

void DrawTree(T* Rt,QGraphicsScene* scene) {
    scene->clear();
    if (!Rt) return;  // treeRoot 是你平衡树的根节点

    double xOffset = 0, step = 2;
    drawTreeNode(Rt, 0, xOffset, step, scene, -1, -1);
}

void QtWidgetsApplicationFinal::clearScene() {
    showSplit(tot-1, Tsg.id, Tsg.time);
    //segments.clear();
    //clickPoints.clear();
    //drawingScene->clear();
    ////currentMode = MODE_SEGMENT;
    //currentMode = MODE_SEGMENT;
    //tot = 0;
    //isPaused = true;      // 是否暂停
    //isStepMode = false;   // 是否单步调试
    //scanLine = nullptr;
}

std::priority_queue <Event> Q;
T* rt[N], *X, *Y, *Z;
double tim[N];

TreapVisualizer::TreapVisualizer(QWidget* parent,int id) : QWidget(parent){
    setWindowTitle("FHQ Treap Visualizer");
    resize(800, 600);
    auto view = new QGraphicsView(this);
    auto scene = new QGraphicsScene(this);

    view->setScene(scene);
    view->setRenderHint(QPainter::Antialiasing);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(view);
    setLayout(layout);

	DrawTree(rt[id], scene);
}
void dfs(int key,T* node,int depth,double xpos,double step,double &x,double &y) {
    if (!node) return;
	if (node->key == key) {
		x = xpos * 60 + 40; // 水平位置
		y = depth * 40 + 10; // 垂直位置
		return;
	}
	dfs(key, node->s[0], depth + 1, xpos - step, step * 0.5,x,y);
	dfs(key, node->s[1], depth + 1, xpos + step, step * 0.5,x,y);
}
void Draw_Color_point(int id, QGraphicsScene* scene, int color,int idx) {
    double x = 0, y = 0;
    dfs(id, rt[idx], 0, 0, 2 ,x,y);
    QColor fillColor = (color == 0) ? QColor(173, 216, 230) : QColor(144, 238, 144);
	int r = 15; // 圆点半径
    auto* point = scene->addEllipse(x - r, y - r, 2 * r, 2 * r,
        QPen(Qt::NoPen), QBrush(fillColor));
    point->setZValue(-1);

    for (int i = 0; i < 6; ++i) {
        point->setVisible(false);
        scene->update();
        QCoreApplication::processEvents();  // 强制刷新
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        point->setVisible(true);
        scene->update();
        QCoreApplication::processEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    //// 闪烁控制变量
    //QTimer::singleShot(200, [=]() { point->setVisible(false); });
    //QTimer::singleShot(400, [=]() { point->setVisible(true); });
    //QTimer::singleShot(600, [=]() { point->setVisible(false); });
    //QTimer::singleShot(800, [=]() { point->setVisible(true); });

    //// 等待动画结束（阻塞式）
    //QEventLoop loop;
    //QTimer::singleShot(2000, &loop, &QEventLoop::quit);  // 2秒后退出事件循环
}

void Draw_line(T* x, T* y, QGraphicsScene* scene,int id) {
	if (!x || !y) return;
	double x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	dfs(x->key, rt[id], 0, 0, 2, x1, y1);
	dfs(y->key, rt[id], 0, 0, 2, x2, y2);
	//QLineF line(QPointF(x1, y1), QPointF(x2, y2));
    double radius = 15;
    double dx = x2 - x1;
    double dy = y2 - y1;
    double len = std::sqrt(dx * dx + dy * dy);
    if (len == 0) return; // 防止除以0

    // 单位向量缩短长度
    double shrinkX = dx / len * radius;
    double shrinkY = dy / len * radius;

    // 缩短两端
    QPointF p1(x1 + shrinkX, y1 + shrinkY);
    QPointF p2(x2 - shrinkX, y2 - shrinkY);

    QLineF line(p1, p2);
    QGraphicsLineItem* lineItem = scene->addLine(line, QPen(Qt::red, 2));
    //QLineF line(QPointF(x1, y1), QPointF(x2, y2));
    //QGraphicsLineItem* lineItem = scene->addLine(line, QPen(Qt::red, 2));
    lineItem->setZValue(-1); // 保证在上层

    for (int i = 0; i < 6; ++i) {
        lineItem->setVisible(false);
        scene->update();
        QCoreApplication::processEvents();  // 强制刷新
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        lineItem->setVisible(true);
        scene->update();
        QCoreApplication::processEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    //QTimer::singleShot(200, [=]() { lineItem->setVisible(false); });
    //QTimer::singleShot(400, [=]() { lineItem->setVisible(true); });
    //QTimer::singleShot(600, [=]() { lineItem->setVisible(false); });
    //QTimer::singleShot(800, [=]() { lineItem->setVisible(true); });

    //// --- 阻塞当前流程但不阻塞 UI：等待动画完成
    //QEventLoop loop;
    //QTimer::singleShot(2000, &loop, SLOT(quit()));
    //loop.exec();
}
void DrawSplit(T* now, int k, T*& x, T*& y, double time, QGraphicsScene* scene,int id) {
    if (!now) { x = y = NULL; return; }
    if (leq(k, now->key, time)) { 
        y = new T(); 
        *y = *now; 
        Draw_Color_point(y->key,scene,1,id);  
        DrawSplit(y->s[0], k, x, y->s[0], time,scene,id);
        Draw_line(y, y->s[0],scene,id);
        y->rub(); 
    }
    else { 
        x = new T(); 
        *x = *now; 
        Draw_Color_point(x->key, scene, 0,id);
        DrawSplit(x->s[1], k, x->s[1], y, time,scene,id);
		Draw_line(x, x->s[1], scene ,id);
        x->rub(); 
    }
}

void QtWidgetsApplicationFinal::showSplit(int id,int nodeid,double time) {
    // 切换到第二个标签页
    tabWidget->setCurrentIndex(1);

    double xOffset = 0, step = 2;
    drawTreeNode(rt[id], 0, xOffset, step, treeScene4, -1, -1);
	T* x = nullptr, * y = nullptr;
	DrawSplit(rt[id], nodeid, x, y ,time , treeScene4,id);
    // 1秒后切换回第一个标签页
    //QTimer::singleShot(1000, this, [=]() {
    //    tabWidget->setCurrentIndex(0);
    //    });
}
void QtWidgetsApplicationFinal::buildTree() {
    //treeScene->clear();
    //treeScene2->clear();
    //treeScene3->clear();
    //treeLayout->clear();
    //treeLayout->addWidget(treeView);
    //treeLayout->addWidget(treeView2);
    //treeLayout->addWidget(treeView3);
    //treeScene->clear();
    currentMode = MODE_BAN;
    QMessageBox::information(this, "tips", "Entered query mode: Click a dot to find the nearest line segment down it");
    int i = 1;
    for (const QLineF& line : segments) {
        x1[i] = line.p1().x();
        x2[i] = line.p2().x();
        Y1[i] = line.p1().y();
        y2[i] = line.p2().y();
        if (x1[i] > x2[i]) std::swap(x1[i], x2[i]), std::swap(Y1[i], y2[i]);
        x2[i] -= EPS;
        //x1[i] -= Eps, Y1[i] -= Eps, x2[i] += Eps, y2[i] += Eps;
        Q.push(Event{ x1[i],i,1 });
        Q.push(Event{ x2[i],i,-1 });
        i++;
    }
    int last = 0;
    while (!Q.empty()) {
        treeScene->clear();
        treeScene2->clear();
        treeScene3->clear();
        auto event = Q.top();
        Tsg = Q.top();
        Q.pop();
        X = Y = Z = nullptr;
        // 平衡树逻辑
        //showSplit(tot,event.id,event.time);
        if (event.type == 1) {
            Spilt2(rt[tot], event.id, X, Y, event.time);
            DrawTree(X, treeScene);
            DrawTree(Y, treeScene2);
            rt[++tot] = Merge(Merge(X, new T(event.id)), Y);
        }
        else {
            Spilt2(rt[tot], event.id, X, Y, event.time);
            DrawTree(X, treeScene);
            DrawTree(Y, treeScene2);
            Spilt(Y, 1, Y, Z);
            rt[++tot] = Merge(X, Z);
        }
        tim[tot] = event.time;
        animateScanLine(last,event.time, 2 ,drawingScene);
        last = event.time;
        // 清空并绘制树
        
        DrawTree(rt[tot], treeScene3);
        //TreapVisualizer* vis = new TreapVisualizer(this,tot-1);  // or nullptr
        //vis->setAttribute(Qt::WA_DeleteOnClose);           // 关闭自动释放内存
        //vis->resize(800, 600);
        //vis->show(); // ✅ 调出窗口
        QApplication::processEvents();

        // 等待“继续”或“下一步”操作
        while (isPaused) {
            QApplication::processEvents();
            QThread::msleep(10);
        }

        // 如果是单步模式，则恢复为暂停状态
        if (isStepMode) {
            isPaused = true;
        }
    }
    currentMode = MODE_POINT_QUERY;
    animateScanLine(last, -1, 2, drawingScene);
    treeScene->clear();
    treeScene2->clear();
    treeScene3->clear();

}

void QtWidgetsApplicationFinal::prevVersion() {
    if (versionIndex > 0) {
        versionIndex--;
        drawTree();
    }
}

void QtWidgetsApplicationFinal::nextVersion() {
    versionIndex++;
    drawTree();
}

void QtWidgetsApplicationFinal::drawTree() {
    treeScene->clear();
    treeScene->addText(QString("Show fhq Treap").arg(versionIndex));
}

int net(T* now, int x, double time) {
    if (!now) return Ma;
    if (leq(now->key, x, time)) return net(now->s[1], x, time);
    int son = net(now->s[0], x, time);
    return leq(now->key, son, time) ? now->key : son;
}

int QtWidgetsApplicationFinal::find_id(QPointF point) {
    double x = point.x(), y = point.y();
    int id = std::upper_bound(tim + 1, tim + tot + 1, x) - tim;
    if (id == 0) return Ma;
    id--;
    //if (tim[id])
    //if (id <= 0)
    x1[0] = 0; Y1[0] = y - eps; x2[0] = 1e9; y2[0] = y - eps;
    int ans = net(rt[id], 0, x);
    DrawTree(rt[id], treeScene);
    return ans;

}

void QtWidgetsApplicationFinal::highlightNearestAbove(QPointF point) {
    int index = find_id(point);
    //QLineF* nearest = nullptr;
    //qreal minDistance = std::numeric_limits<qreal>::max();
    //int index = 0, best_index = 0;
    //for (QLineF& line : segments) {
    //    // 检查线段是否在该点上方，且横跨该点的 x
    //    if (line.x1() <= point.x() && line.x2() >= point.x() ||
    //        line.x2() <= point.x() && line.x1() >= point.x()) {

    //        // 线性插值算出该线段在 point.x 的 y 值
    //        qreal dx = line.x2() - line.x1();
    //        if (dx == 0) continue; // 垂直线段跳过
    //        qreal t = (point.x() - line.x1()) / dx;
    //        qreal yOnLine = line.y1() + t * (line.y2() - line.y1());

    //        if (yOnLine < point.y()) continue; // 必须在线段上方

    //        qreal dy = yOnLine - point.y();
    //        if (dy < minDistance) {
    //            minDistance = dy;
    //            nearest = &line;
    //            best_index = index;
    //        }
    //    
    //    }
    //    index++;
    //}

    drawingScene->clear();
    //QMessageBox::information(this, "tips", QString("%1").arg(index));
    int idx = 1;
    for (const QLineF& line : segments) {
        QPen pen = QPen(Qt::darkGray, 2);
        if (idx == index) {
            pen = QPen(Qt::red, 3);  // 高亮线段
        }
        drawingScene->addLine(line, pen);

        // 起点/终点圆圈 & 标签照旧
        QRectF startCircle(line.p1().x() - 5, line.p1().y() - 5, 10, 10);
        auto* startDot = drawingScene->addEllipse(startCircle, QPen(Qt::black), QBrush(Qt::white));
        startDot->setToolTip(QString("起点 (%1, %2)").arg(line.p1().x(), 0, 'f', 1).arg(line.p1().y(), 0, 'f', 1));

        QRectF endCircle(line.p2().x() - 5, line.p2().y() - 5, 10, 10);
        auto* endDot = drawingScene->addEllipse(endCircle, QPen(Qt::black), QBrush(Qt::white));
        endDot->setToolTip(QString("终点 (%1, %2)").arg(line.p2().x(), 0, 'f', 1).arg(line.p2().y(), 0, 'f', 1));

        auto* label = drawingScene->addText(QString::number(idx));
        label->setDefaultTextColor(Qt::blue);
        label->setFont(QFont("Arial", 10, QFont::Bold));
        label->setPos((line.p1().x() + line.p2().x()) / 2, (line.p1().y() + line.p2().y()) / 2);
        ++idx;
    }

    // 显示查询点
    drawingScene->addEllipse(point.x() - 3, point.y() - 3, 6, 6, QPen(Qt::blue), QBrush(Qt::blue));
}