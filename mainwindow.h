#pragma once

#include <QWidget>

#include <QVTKOpenGLWidget.h>

#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

class MainWindow : public QVTKOpenGLWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

private:
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkRenderWindow> renderWindow;
};
