#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "camera.h"
#include "cursorgrid.h"
#include "rotationfinder.h"

#include <QEvent>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_cameraName_currentTextChanged(const QString &arg1);

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    Camera camera;
    CursorGrid cursorGrid;
    RotationFinder rotationFinder;
    QSettings settings;

    QThread rotationFinderThread;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *object, QEvent *event) override;
};
#endif // MAINWINDOW_H
