#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QSerialPort>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void sendCommand(const QString &command);

private:
    Ui::MainWindow *ui;

    // --- Persistent QSerialPort ---
    QSerialPort serial;

    bool lightsOn = false;
    int appliedIntensity = 75;
    int appliedMoveTimerSec = 60;

    enum class Mode { Automatic, Motion };
    Mode appliedMode = Mode::Automatic;

    QByteArray serialBuffer;

    void updateStatusLabel();
    void updatePowerButton();

private slots:
    void displayFeedback(const QString &msg);

    void on_buttonStart_clicked();
    void on_buttonAutomatic_clicked();
    void on_buttonMotion_clicked();
    void on_buttonBackFromAutomatic_clicked();
    void on_buttonBackFromMotion_clicked();

    void on_buttonPowerToggle_toggled(bool checked);

    void on_buttonConfirmAutomatic_clicked();
    void on_buttonConfirmMotion_clicked();

    void on_sliderAutoIntensity_valueChanged(int value);
    void on_sliderMoveIntensity_valueChanged(int value);
    void on_sliderMoveTimer_valueChanged(int value);
};

#endif // MAINWINDOW_H
