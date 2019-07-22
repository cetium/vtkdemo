#pragma once

#include <QWidget>

#include <QVTKOpenGLWidget.h>

class MainWindow : public QVTKOpenGLWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *e);
};
