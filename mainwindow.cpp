#include "mainwindow.h"
#include <QDebug>
#include <QDirIterator>
#include <QFileDialog>
#include <QWindow>

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkInteractionStyle)

MainWindow::MainWindow(QWidget *parent) : QVTKOpenGLWidget(parent) {}

MainWindow::~MainWindow() {
}

void MainWindow::closeEvent(QCloseEvent *e) {
}
