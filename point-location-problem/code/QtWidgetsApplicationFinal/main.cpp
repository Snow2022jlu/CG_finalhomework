#include "QtWidgetsApplicationFinal.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QtWidgetsApplicationFinal window;
    QFont font("SimSun", 10);  // �� "SimSun", "Arial Unicode MS" ��
    QApplication::setFont(font);
    window.show();
    return app.exec();
}
