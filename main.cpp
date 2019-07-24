#include "mainwindow.h"
#include <QApplication>

#include <QSurfaceFormat>

#include <vtkOpenGLRenderWindow.h>

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    vtkOpenGLRenderWindow::SetGlobalMaximumNumberOfMultiSamples(8);      //1
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat()); //2

    MainWindow view;
    view.show();

    view.resize(800, 600);
    return app.exec();
}
