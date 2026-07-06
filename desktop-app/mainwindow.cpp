#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>
#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QRegularExpression>

namespace {
constexpr int PAGE_WELCOME = 0;
constexpr int PAGE_MENU = 1;
constexpr int PAGE_AUTOMATIC = 2;
constexpr int PAGE_MOTION = 3;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // --- Serial Setup ---
    serial.setPortName(QStringLiteral("COM3"));
    serial.setBaudRate(QSerialPort::Baud9600);

    if (!serial.open(QIODevice::ReadWrite)) {
        qDebug() << "FAILED TO OPEN PORT ON STARTUP:" << serial.errorString();
    } else {
        qDebug() << "Serial port opened successfully.";
    }

    if (serial.isOpen()) {
        sendCommand("E\n");
    }

    connect(&serial, &QSerialPort::readyRead, this, [this]() {
        serialBuffer.append(serial.readAll());

        int idx = 0;
        while ((idx = serialBuffer.indexOf('\n')) != -1) {
            QString line = QString::fromUtf8(serialBuffer.left(idx)).trimmed();
            serialBuffer.remove(0, idx + 1);

            if (!line.isEmpty()) {
                displayFeedback(line);
            }
        }
    });

    if (ui->stackedWidget)
        ui->stackedWidget->setCurrentIndex(PAGE_WELCOME);
}

MainWindow::~MainWindow()
{
    if (serial.isOpen())
        serial.close();

    delete ui;
}

void MainWindow::updateStatusLabel()
{
    if (!ui->labelStatus)
        return;

    const bool isAutomatic = appliedMode == Mode::Automatic;

    QString status = QString("Mode: %1 • Intensity: %2%")
                         .arg(isAutomatic ? "Automatic" : "Motion")
                         .arg(appliedIntensity);

    if (!isAutomatic)
        status += QString(" • Timer: %1 s").arg(appliedMoveTimerSec);

    ui->labelStatus->setText(status);
}

// --------------------------- PAGE NAVIGATION ---------------------------

void MainWindow::on_buttonStart_clicked()
{
    ui->stackedWidget->setCurrentIndex(PAGE_MENU);

    ui->buttonPowerToggle->setChecked(lightsOn);
    updatePowerButton();

    updateStatusLabel();
}

void MainWindow::on_buttonAutomatic_clicked()
{
    ui->sliderAutoIntensity->setValue(appliedIntensity);
    ui->labelAutoIntensity->setText(QString("Light intensity: %1%").arg(appliedIntensity));

    ui->stackedWidget->setCurrentIndex(PAGE_AUTOMATIC);
}

void MainWindow::on_buttonMotion_clicked()
{
    ui->sliderMoveIntensity->setValue(appliedIntensity);
    ui->labelMoveIntensity->setText(QString("Light intensity: %1%").arg(appliedIntensity));

    ui->sliderMoveTimer->setValue(appliedMoveTimerSec);
    ui->labelMoveTimer->setText(QString("Timer: %1 s").arg(appliedMoveTimerSec));

    ui->stackedWidget->setCurrentIndex(PAGE_MOTION);
}

void MainWindow::on_buttonBackFromAutomatic_clicked()
{
    ui->stackedWidget->setCurrentIndex(PAGE_MENU);
    updateStatusLabel();
}

void MainWindow::on_buttonBackFromMotion_clicked()
{
    ui->stackedWidget->setCurrentIndex(PAGE_MENU);
    updateStatusLabel();
}

// --------------------------- POWER BUTTON ---------------------------

void MainWindow::updatePowerButton()
{
    if (!ui->buttonPowerToggle)
        return;

    if (lightsOn)
    {
        ui->buttonPowerToggle->setText("On");
        ui->buttonPowerToggle->setStyleSheet(
            "background-color: #28a745; color: white; font-weight: bold;"
            );
    }
    else
    {
        ui->buttonPowerToggle->setText("Off");
        ui->buttonPowerToggle->setStyleSheet(
            "background-color: #dc3545; color: white; font-weight: bold;"
            );
    }
}

void MainWindow::on_buttonPowerToggle_toggled(bool checked)
{
    lightsOn = checked;

    sendCommand(lightsOn ? "D1\n" : "D0\n");

    updatePowerButton();
}

// --------------------------- CONFIRM BUTTONS ---------------------------

void MainWindow::on_buttonConfirmAutomatic_clicked()
{
    appliedIntensity = ui->sliderAutoIntensity->value();
    appliedMode = Mode::Automatic;

    sendCommand("A0\n");
    QThread::msleep(300);
    sendCommand(QString("B%1\n").arg(appliedIntensity));
    QThread::msleep(300);

    on_buttonBackFromAutomatic_clicked();
}

void MainWindow::on_buttonConfirmMotion_clicked()
{
    appliedIntensity = ui->sliderMoveIntensity->value();
    appliedMoveTimerSec = ui->sliderMoveTimer->value();
    appliedMode = Mode::Motion;

    sendCommand("A1\n");
    QThread::msleep(300);

    sendCommand(QString("B%1\n").arg(appliedIntensity));
    QThread::msleep(300);

    sendCommand(QString("C%1\n").arg(appliedMoveTimerSec));
    QThread::msleep(300);

    on_buttonBackFromMotion_clicked();
}

// --------------------------- SLIDERS ---------------------------

void MainWindow::on_sliderAutoIntensity_valueChanged(int value)
{
    ui->labelAutoIntensity->setText(QString("Light intensity: %1%").arg(value));
}

void MainWindow::on_sliderMoveIntensity_valueChanged(int value)
{
    ui->labelMoveIntensity->setText(QString("Light intensity: %1%").arg(value));
}

void MainWindow::on_sliderMoveTimer_valueChanged(int value)
{
    ui->labelMoveTimer->setText(QString("Timer: %1 s").arg(value));
}

// --------------------------- SERIAL OUT ---------------------------

void MainWindow::sendCommand(const QString &command)
{
    QByteArray out = command.toUtf8();

    if (!serial.isOpen()) {
        qDebug() << "Port is not open! Cannot send.";
        return;
    }

    qDebug() << "[SEND] STRING:" << command.trimmed();

    serial.write(out);
    serial.flush();
}

// --------------------------- SERIAL IN ---------------------------

void MainWindow::displayFeedback(const QString &msg)
{
    // Power
    if (msg.contains("D1")) lightsOn = true;
    if (msg.contains("D0")) lightsOn = false;
    updatePowerButton();

    // Mode
    if (msg.contains("A0")) appliedMode = Mode::Automatic;
    if (msg.contains("A1")) appliedMode = Mode::Motion;

    // Intensity
    QRegularExpression reI("B(\\d+)");
    auto mI = reI.match(msg);
    if (mI.hasMatch())
        appliedIntensity = mI.captured(1).toInt();

    // Timer
    QRegularExpression reT("C(\\d+)");
    auto mT = reT.match(msg);
    if (mT.hasMatch())
        appliedMoveTimerSec = mT.captured(1).toInt();

    // Update GUI
    ui->buttonPowerToggle->setChecked(lightsOn);
    updatePowerButton();

    ui->sliderAutoIntensity->setValue(appliedIntensity);
    ui->labelAutoIntensity->setText(QString("Light intensity: %1%").arg(appliedIntensity));

    ui->sliderMoveIntensity->setValue(appliedIntensity);
    ui->labelMoveIntensity->setText(QString("Light intensity: %1%").arg(appliedIntensity));

    ui->sliderMoveTimer->setValue(appliedMoveTimerSec);
    ui->labelMoveTimer->setText(QString("Timer: %1 s").arg(appliedMoveTimerSec));

    updateStatusLabel();
}
