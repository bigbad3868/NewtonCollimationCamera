#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QSettings>

#include <QList>
#include <QPair>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , settings("AsiCollimator")
{
    ui->setupUi(this);

    rotationFinder.moveToThread(&rotationFinderThread);
    rotationFinderThread.start();

    ui->graphicsView->setScene(new QGraphicsScene(this));
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->graphicsView->viewport()->installEventFilter(this);

    // Cursor Offset X and Y
    QObject::connect(&cursorGrid, &CursorGrid::offsetXChanged, ui->offsetX, &QSpinBox::setValue);
    QObject::connect(&cursorGrid, &CursorGrid::offsetYChanged, ui->offsetY, &QSpinBox::setValue);

    QObject::connect(ui->offsetX, qOverload<int>(&QSpinBox::valueChanged), &cursorGrid, &CursorGrid::setOffsetX);
    QObject::connect(ui->offsetY, qOverload<int>(&QSpinBox::valueChanged), &cursorGrid, &CursorGrid::setOffsetY);

    cursorGrid.setOffsetX(settings.value("offset/x", "0").toInt());
    cursorGrid.setOffsetY(settings.value("offset/y", "0").toInt());

    // Camera Video Data
    connect(&camera, &Camera::onFrame, this, [&](QImage image){
        ui->graphicsView->scene()->clear();
        ui->graphicsView->scene()->setSceneRect(0, 0, image.width(), image.height());
        ui->graphicsView->scene()->addPixmap(QPixmap::fromImage(image));
        if (ui->markerCenter->isChecked())
        {
            cursorGrid.draw(ui->graphicsView->scene());
        }

    }, Qt::QueuedConnection);

    connect(ui->autoCenter, &QGroupBox::toggled, this, [&](bool checked)
    {
        if (checked)
        {
            rotationFinder.reset();
        }
        else
        {
            rotationFinder.stop();
        }
    });

    connect(&camera, &Camera::onFrame, &rotationFinder, &RotationFinder::process);

    connect(&rotationFinder, &RotationFinder::completed, this, [&](double quality, double rotation, QPoint centerOfRotation){

        const bool isGood = qAbs(rotation) > 10 && qAbs(rotation) < 90;
        ui->autoCenterRotation->setText(QString::number(rotation, 'f', 3));
        ui->autoCenterQuality->setText(QString::number(quality * 100, 'f', 1) + "%");
        if (isGood)
        {
            ui->offsetX->setValue(centerOfRotation.x());
            ui->offsetY->setValue(centerOfRotation.y());
        }
    });

    // Sliders / circle
    QList<QPair<QSlider*, QLabel*>> rings = {
        {ui->ringSlider_1, ui->ringValue_1},
        {ui->ringSlider_2, ui->ringValue_2},
        {ui->ringSlider_3, ui->ringValue_3},
        {ui->ringSlider_4, ui->ringValue_4}
    };

    int index = 0;
    for (const auto &it : rings)
    {
        int value = settings.value(QString("ring/%1").arg(index), "0").toInt();

        auto ringSlider = it.first;
        auto ringValue = it.second;


        connect(ringSlider, &QSlider::valueChanged, [this, index, ringValue](int value){
            cursorGrid.rings[index] = value;
            ringValue->setText(QString("%1 px").arg(value));
            settings.setValue(QString("ring/%1").arg(index), value);
        });

        cursorGrid.rings.append(value);
        ringSlider->setValue(value);

        index++;
    }

    // Gain
    connect(ui->gainSlider, &QSlider::valueChanged, &camera, &Camera::setGain);
    connect(&camera, &Camera::gainChanged, [&](int value){
        ui->gainSlider->setValue(value);
        ui->gainValue->setText(QString("%1 x").arg(value));
    });

    emit camera.gainChanged(camera.gain());

    // Exposure
    connect(ui->exposureSlider, &QSlider::valueChanged, &camera, &Camera::setExposure);
    connect(&camera, &Camera::exposureChanged, [&](int value){
        ui->exposureSlider->setValue(value);
        ui->exposureValue->setText(QString("%1 ms").arg(value / 1000));
    });

    emit camera.exposureChanged(camera.exposure());

    // Camera List Names
    QObject::connect(ui->cameraNameRefresh, &QPushButton::pressed, [&](){
        ui->cameraName->blockSignals(true);
        ui->cameraName->clear();
        ui->cameraName->addItem("--- select camera ---");
        for (const auto &it: camera.availableCameras())
        {
            ui->cameraName->addItem(it);
        }
        ui->cameraName->blockSignals(false);
    });
    ui->cameraNameRefresh->click(); // ;)
}

MainWindow::~MainWindow()
{
    rotationFinderThread.quit();
    settings.setValue("offset/x", cursorGrid.offsetX());
    settings.setValue("offset/y", cursorGrid.offsetY());
    settings.sync();
    delete ui;
}

#include <QKeyEvent>
#include <QtMath>

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int step = event->modifiers() & Qt::ShiftModifier ? 10 : 1;
    if (event->key() == Qt::Key_W) cursorGrid.addOffsetY( - step);
    if (event->key() == Qt::Key_S) cursorGrid.addOffsetY( + step);
    if (event->key() == Qt::Key_A) cursorGrid.addOffsetX( - step);
    if (event->key() == Qt::Key_D) cursorGrid.addOffsetX( + step);
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    static double value = 1;
    if (event->type() == QEvent::Wheel)
    {
        auto wheelEvent = static_cast<QWheelEvent*>(event);
        value += wheelEvent->angleDelta().y() / 1000.0;
        value = qMin(qMax(value, qSqrt(0.1)), qSqrt(20.0));

        double scale = value * value;
        ui->graphicsView->resetTransform();
        ui->graphicsView->scale(scale,  scale);
        cursorGrid.setWidth(1.0 / scale);
        return true;
    }
    return QObject::eventFilter(object, event);
}

void MainWindow::on_cameraName_currentTextChanged(const QString &arg1)
{
    camera.setCamera(arg1);
}

#include <QFileDialog>
#include <QDateTime>

void MainWindow::on_pushButton_clicked()
{
    auto scene = ui->graphicsView->scene();

    scene->clearSelection();
    scene->setSceneRect(scene->itemsBoundingRect());
    QImage image(scene->sceneRect().size().toSize(), QImage::Format_RGB32);
    //image.fill(Qt::transparent);

    auto fileName = QFileDialog::getSaveFileName(
        this,
        "Save File",
        QDir::homePath() + QString("/collimation_%1.png").arg(QDateTime::currentDateTime().toString("yyMMdd-hhmmss")),
        "PNG Image(*.png)::Any File (*.*)"
    );

    QPainter painter(&image);
    scene->render(&painter);
    image.save(fileName);
}
