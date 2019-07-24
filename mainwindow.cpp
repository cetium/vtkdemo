#include "mainwindow.h"
#include <QDebug>
#include <QDirIterator>
#include <QFileDialog>
#include <QWindow>
#include <qevent.h>

#include <vtkAxesActor.h>
#include <vtkBooleanOperationPolyDataFilter.h>
#include <vtkCamera.h>
#include <vtkCleanPolyData.h>
#include <vtkClipVolume.h>
#include <vtkColorTransferFunction.h>
#include <vtkContourFilter.h>
#include <vtkDiscreteMarchingCubes.h>
#include <vtkExtractVOI.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMarchingCubes.h>
#include <vtkImageProperty.h>
#include <vtkImageReslice.h>
#include <vtkImageSliceMapper.h>
#include <vtkImageViewer2.h>
#include <vtkInteractorStyleImage.h>
#include <vtkIntersectionPolyDataFilter.h>
#include <vtkMarchingCubes.h>
#include <vtkMassProperties.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkSphereSource.h>
#include <vtkSurfaceReconstructionFilter.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>
#include <vtkVolumeProperty.h>

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2)

MainWindow::MainWindow(QWidget *parent) : QVTKOpenGLWidget(parent) {
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->SetBackground(0.9, 0.2, 0.2);

    vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
    camera->SetPosition(5, 0, 0);
    camera->SetFocalPoint(0, 0, 10);
    renderer->SetActiveCamera(camera);
    renderer->GetActiveCamera()->Azimuth(45);
    renderer->GetActiveCamera()->Elevation(-45);
    renderer->ResetCamera();

    renderWindow = GetRenderWindow(); //vtkSmartPointer<vtkRenderWindow>::New();
    //SetRenderWindow(renderWindow);

    renderWindow->AddRenderer(renderer);
}

MainWindow::~MainWindow() {}

void MainWindow::closeEvent(QCloseEvent *e) {}

const int Resolution = 80;

void MainWindow::keyReleaseEvent(QKeyEvent *e) {
    qDebug() << "keyEvent" << e->key();
    switch (e->key()) {
    case Qt::Key_F1: {
        renderer->RemoveAllViewProps();
        // Sphere
        vtkNew<vtkSphereSource> sphere1, sphere2;
        sphere1->SetCenter(0, 0, 0);
        sphere1->SetRadius(8);
        sphere1->SetThetaResolution(Resolution);
        sphere1->SetPhiResolution(Resolution);
        sphere1->Update();
        sphere2->SetCenter(10, 0, 0);
        sphere2->SetRadius(8);
        sphere2->SetThetaResolution(Resolution);
        sphere2->SetPhiResolution(Resolution);
        sphere2->Update();

        vtkNew<vtkPolyDataMapper> sphereMapper1, sphereMapper2;
        sphereMapper1->SetInputConnection(sphere1->GetOutputPort());
        sphereMapper2->SetInputConnection(sphere2->GetOutputPort());
        sphereMapper2->ScalarVisibilityOn();
        vtkNew<vtkActor> sphereActor1, sphereActor2;
        sphereActor1->SetMapper(sphereMapper1.Get());
        sphereActor2->SetMapper(sphereMapper2.Get());

        renderer->AddActor(sphereActor1);
        renderer->AddActor(sphereActor2);

        renderWindow->Render();
    } break;
    case Qt::Key_F2: {
        renderer->RemoveAllViewProps();

        // Sphere Intersection

        vtkNew<vtkSphereSource> sphere1, sphere2;
        sphere1->SetCenter(0, 0, 0);
        sphere1->SetRadius(8);
        sphere1->SetThetaResolution(Resolution);
        sphere1->SetPhiResolution(Resolution);
        sphere1->Update();
        sphere2->SetCenter(10, 0, 0);
        sphere2->SetRadius(8);
        sphere2->SetThetaResolution(Resolution);
        sphere2->SetPhiResolution(Resolution);
        sphere2->Update();

        vtkNew<vtkBooleanOperationPolyDataFilter> intersectionPolyDataFilter;
        intersectionPolyDataFilter->SetOperationToIntersection();
        intersectionPolyDataFilter->SetInputConnection(0, sphere1->GetOutputPort());
        intersectionPolyDataFilter->SetInputConnection(1, sphere2->GetOutputPort());
        intersectionPolyDataFilter->Update();

        vtkNew<vtkPolyDataMapper> intersectionMapper;
        intersectionMapper->SetInputConnection(intersectionPolyDataFilter->GetOutputPort());
        intersectionMapper->ScalarVisibilityOff();

        vtkNew<vtkActor> intersectionActor;
        intersectionActor->SetMapper(intersectionMapper);

        vtkNew<vtkMassProperties> mass;
        mass->SetInputData(intersectionPolyDataFilter->GetOutput());
        vtkNew<vtkTextActor> text;
        char buf[128];
        sprintf(buf, "volume=%.3f; suface area=%.3f", mass->GetVolume(), mass->GetSurfaceArea());
        text->SetInput(buf);
        text->SetPosition2(50, 20);
        text->GetTextProperty()->SetFontSize(20);
        text->GetTextProperty()->SetColor(0.0, 0.0, 1.0);

        renderer->AddActor(intersectionActor);
        renderer->AddActor(text);

        renderWindow->Render();
    } break;
    case Qt::Key_F3: {
        renderer->RemoveAllViewProps();

        vtkNew<vtkImageData> img;
        img->SetDimensions(100, 100, 100);
        img->SetOrigin(0, 0, 0);
        img->SetSpacing(1, 1, 1);
        img->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
        img->GetPointData()->GetScalars()->Fill(0);

        int *dim = img->GetDimensions();

        vtkNew<vtkExtractVOI> voi;
        voi->SetInputData(img);
        voi->SetVOI(20, 30, 20, 30, 20, 30);
        voi->SetSampleRate(1, 1, 1);
        voi->Update();
        vtkSmartPointer<vtkImageData> imgCube = voi->GetOutput();
        imgCube->GetPointData()->GetScalars()->Fill(255);

        double center[3] = {25, 25, 25};
        vtkNew<vtkTransform> mtx;
        mtx->Translate(center[0], center[1], center[2]);
        mtx->RotateX(30);
        mtx->Translate(-center[0], -center[1], -center[2]);

        vtkNew<vtkImageReslice> reslice;
        reslice->SetInputData(imgCube);
        reslice->SetResliceTransform(mtx);
        reslice->SetInterpolationModeToCubic();
        //reslice->SetOutputExtent(img->GetExtent());
        reslice->Update();

        vtkSmartPointer<vtkImageData> imgCube2 = reslice->GetOutput();

        //vtkNew<vtkImageMarchingCubes> mc;
        //vtkNew<vtkMarchingCubes> mc;
        vtkNew<vtkDiscreteMarchingCubes> mc;
        mc->SetInputData(imgCube2);
        //mc->ComputeNormalsOn();
        //mc->GenerateValues(1, 0, 255);
        mc->Update();

        vtkNew<vtkCleanPolyData> cleaner;
        cleaner->SetInputData(mc->GetOutput());
        cleaner->SetAbsoluteTolerance(1E-3);
        cleaner->Update();

        //mc->SetValue(0, 255);
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputConnection(mc->GetOutputPort());
        mapper->ScalarVisibilityOff();

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);

        vtkNew<vtkImageActor> imageActor, imageActor2;
        imageActor->SetInputData(img);
        imageActor2->SetInputData(imgCube2);

        /*vtkNew<vtkImageViewer2> imageViewer;
        imageViewer->SetSliceOrientation(0);
        imageViewer->SetInputData(imgCube2);
        vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
        imageViewer->SetupInteractor(renderWindowInteractor);
        imageViewer->Render();
        imageViewer->GetRenderer()->ResetCamera();
        imageViewer->Render();
        renderWindowInteractor->Start();*/

        class vtkTimerCallback2 : public vtkCommand {
        public:
            static vtkTimerCallback2 *New() {
                vtkTimerCallback2 *cb = new vtkTimerCallback2;
                return cb;
            }

            virtual void Execute(vtkObject *caller, unsigned long eventId, void *vtkNotUsed(callData)) {
                vtkRenderWindowInteractor *iren = dynamic_cast<vtkRenderWindowInteractor *>(caller);

                actor->RotateZ(36);
                iren->GetRenderWindow()->Render();
            }

        public:
            vtkActor *actor;
        };

        auto renderWindowInteractor = renderWindow->GetInteractor();
        vtkNew<vtkTimerCallback2> cb;
        cb->actor = actor;
        actor->SetOrigin(center);
        renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, cb);

        int timerId = renderWindowInteractor->CreateRepeatingTimer(100);

        renderer->AddActor(imageActor);
        renderer->AddActor(imageActor2);
        renderer->AddActor(actor);

        renderWindow->Render();
    } break;
    default:
        break;
    }
}
