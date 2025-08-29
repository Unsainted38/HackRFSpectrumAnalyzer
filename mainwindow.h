#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "qcustomplot/qcustomplot.h"
#include <algorithm>
#include <QElapsedTimer>

#include "hackrfdevice.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_Freq_pushButton_clicked();

    void on_SampleRate_pushButton_clicked();

    void onTimeout();

    void on_startrx_pushButton_clicked();

    void on_stoprx_pushButton_clicked();

    void on_Freq_horizontalSlider_sliderMoved(int position);

    void on_Rbw_pushButton_clicked();

    void on_SampleRate_horizontalSlider_sliderMoved(int position);

    void on_Rbw_horizontalSlider_sliderMoved(int position);

    void on_Freq_doubleSpinBox_valueChanged(double arg1);

    void on_SampleRate_doubleSpinBox_valueChanged(double arg1);

    void on_Rbw_doubleSpinBox_valueChanged(double arg1);

    void onSampleRateChanged();
    void onCenterFreqChanged();
    void onSpanChanged();
    //void onRBWChanged();

    void on_Vbw_doubleSpinBox_valueChanged(double arg1);

    void on_Vbw_pushButton_clicked();

    void on_Vbw_horizontalSlider_valueChanged(int value);

    void onCenterPowerReady(double pwr);

    void onIntegralPowerReady(double pwr);

    void updateSpectrumPlot(const QVector<double> &spectrum, double sampleRate, double centerFreq, double span);

    void on_LNA_Gain_horizontalSlider_valueChanged(int value);

    void on_LNA_Gain_doubleSpinBox_valueChanged(double arg1);

    void on_LNA_Gain_pushButton_clicked();

    void on_VGA_Gain_horizontalSlider_valueChanged(int value);

    void on_VGA_Gain_doubleSpinBox_valueChanged(double arg1);

    void on_VGA_Gain_pushButton_clicked();

    void on_Span_pushButton_clicked();

    void on_Span_horizontalSlider_valueChanged(int value);

    void on_Span_doubleSpinBox_valueChanged(double arg1);

private:
    Ui::MainWindow *ui;
    HackRFDevice *device;
    QTimer timer;
    QElapsedTimer plotTimer;

    void DisableSetupLayout();
    void EnableSetupLayout();
    double getK(QString name);
};
#endif // MAINWINDOW_H
