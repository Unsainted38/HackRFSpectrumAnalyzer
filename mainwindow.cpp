#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot/qcustomplot.h"
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    device = new HackRFDevice();
    device->open();
    ui->Freq_doubleSpinBox->setValue(device->getCenterFrequency() * 0.001);
    ui->Freq_horizontalSlider->setValue(device->getCenterFrequency() * 0.001);
    ui->SampleRate_doubleSpinBox->setValue(device->getSampleRate() * 0.000001);
    ui->SampleRate_horizontalSlider->setValue(device->getSampleRate() * 0.000001);
    ui->Rbw_doubleSpinBox->setValue(device->getRBW());
    ui->Rbw_horizontalSlider->setValue(device->getRBW());
    ui->Vbw_doubleSpinBox->setValue(device->getVBW());
    ui->Vbw_horizontalSlider->setValue(device->getVBW());
    ui->LNA_Gain_doubleSpinBox->setValue(device->getLnaGain());
    ui->LNA_Gain_horizontalSlider->setValue(device->getLnaGain());
    ui->VGA_Gain_doubleSpinBox->setValue(device->getVgaGain());
    ui->VGA_Gain_horizontalSlider->setValue(device->getVgaGain());
    connect(device, SIGNAL(sampleRateChanged()), this, SLOT(onSampleRateChanged()));
    connect(device, SIGNAL(centerFreqChanged()), this, SLOT(onCenterFreqChanged()));
    connect(device, SIGNAL(spanChanged()), this, SLOT(onSpanChanged()));
    connect(device, SIGNAL(centralPowerReady(double)), this, SLOT(onCenterPowerReady(double)));
    connect(device, SIGNAL(integralPowerReady(double)), this, SLOT(onIntegralPowerReady(double)));
    connect(device,  &HackRFDevice::spectrumReady,
            this, &MainWindow::updateSpectrumPlot,
            Qt::QueuedConnection);
    ui->plotSpectrum->addGraph();
    ui->plotSpectrum->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->plotSpectrum->graph(0)->setPen(QPen(Qt::green));
    ui->plotSpectrum->xAxis->setLabel("Frequency (Hz)");
    ui->plotSpectrum->yAxis->setLabel("Amplitude (dB)");
    ui->plotSpectrum->rescaleAxes();
    ui->plotSpectrum->xAxis->setRange(device->getCenterFrequency() - device->getSampleRate() / 2, device->getCenterFrequency() + device->getSampleRate() / 2);
    ui->plotSpectrum->replot();
    ui->plotSpectrum->update();
}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::on_Freq_pushButton_clicked() {
    device->setCenterFrequency(ui->Freq_horizontalSlider->value() * getK(ui->Freq_comboBox->currentText()));
}


void MainWindow::on_SampleRate_pushButton_clicked() {
    device->setSampleRate(ui->SampleRate_horizontalSlider->value() * getK(ui->SampleRate_comboBox->currentText()));
}

void MainWindow::onTimeout() {

}

void MainWindow::on_startrx_pushButton_clicked() {
    device->start();
    plotTimer.start();
    //DisableSetupLayout();
}


void MainWindow::on_stoprx_pushButton_clicked() {
    device->stop();
    ui->power_progressBar->reset();
    ui->power_label->setText("-");
    //EnableSetupLayout();
}


void MainWindow::on_Freq_horizontalSlider_sliderMoved(int position) {
    ui->Freq_doubleSpinBox->setValue((double)position);
}

void MainWindow::DisableSetupLayout() {
    ui->Freq_doubleSpinBox->setEnabled(false);
    ui->SampleRate_doubleSpinBox->setEnabled(false);
    ui->Rbw_doubleSpinBox->setEnabled(false);
    ui->Freq_horizontalSlider->setEnabled(false);
    ui->SampleRate_horizontalSlider->setEnabled(false);
    ui->Rbw_horizontalSlider->setEnabled(false);
    ui->Freq_pushButton->setEnabled(false);
    ui->SampleRate_pushButton->setEnabled(false);
    ui->Rbw_pushButton->setEnabled(false);
}

void MainWindow::EnableSetupLayout() {
    ui->Freq_doubleSpinBox->setEnabled(true);
    ui->SampleRate_doubleSpinBox->setEnabled(true);
    ui->Rbw_doubleSpinBox->setEnabled(true);
    ui->Freq_horizontalSlider->setEnabled(true);
    ui->SampleRate_horizontalSlider->setEnabled(true);
    ui->Rbw_horizontalSlider->setEnabled(true);
    ui->Freq_pushButton->setEnabled(true);
    ui->SampleRate_pushButton->setEnabled(true);
    ui->Rbw_pushButton->setEnabled(true);
}

double MainWindow::getK(QString name) {
    if(name == "Hz") {
        return 1.0;
    } else if(name == "KHz") {
        return 1e3;
    } else if(name == "MHz") {
        return 1e6;
    } else if(name == "GHz") {
        return 1e9;
    }
    return 0.0;
}


void MainWindow::on_Rbw_pushButton_clicked() {
    device->setRBW(ui->Rbw_doubleSpinBox->value() * getK(ui->Rbw_comboBox->currentText()));
}


void MainWindow::on_SampleRate_horizontalSlider_sliderMoved(int position) {
    ui->SampleRate_doubleSpinBox->setValue((double)position);
}


void MainWindow::on_Rbw_horizontalSlider_sliderMoved(int position) {
    ui->Rbw_doubleSpinBox->setValue((double)position);
}


void MainWindow::on_Freq_doubleSpinBox_valueChanged(double arg1) {
    ui->Freq_horizontalSlider->setValue((int)arg1);
}


void MainWindow::on_SampleRate_doubleSpinBox_valueChanged(double arg1) {
    ui->SampleRate_horizontalSlider->setValue((int)arg1);
}


void MainWindow::on_Rbw_doubleSpinBox_valueChanged(double arg1) {
    ui->Rbw_horizontalSlider->setValue((int)arg1);
}

void MainWindow::onSampleRateChanged() {
    ui->plotSpectrum->xAxis->setRange(device->getCenterFrequency() - device->getSampleRate() / 2, device->getCenterFrequency() + device->getSampleRate() / 2);
    ui->plotSpectrum->replot();
}

void MainWindow::onCenterFreqChanged() {
    ui->plotSpectrum->xAxis->setRange(device->getCenterFrequency() - device->getSampleRate() / 2, device->getCenterFrequency() + device->getSampleRate() / 2);
    ui->plotSpectrum->replot();
}

void MainWindow::onSpanChanged() {
    ui->plotSpectrum->xAxis->setRange(device->getCenterFrequency() - device->getSpan() / 2, device->getCenterFrequency() + device->getSpan() / 2);
    ui->plotSpectrum->replot();
}


void MainWindow::on_Vbw_doubleSpinBox_valueChanged(double arg1) {
    ui->Vbw_horizontalSlider->setValue((int)arg1);
}


void MainWindow::on_Vbw_pushButton_clicked() {
    device->setVBW(ui->Vbw_doubleSpinBox->value());
}


void MainWindow::on_Vbw_horizontalSlider_valueChanged(int value) {
    ui->Vbw_doubleSpinBox->setValue((double)value);
}

void MainWindow::onCenterPowerReady(double pwr) {
    ui->power_label->setText(QString::number(pwr));
    ui->power_progressBar->setValue(pwr);
}

void MainWindow::onIntegralPowerReady(double pwr) {
    ui->integral_power_label->setText(QString::number(pwr));
    ui->integral_power_progressBar->setValue(pwr);
}



void MainWindow::updateSpectrumPlot(const QVector<double> &spectrum, double sampleRate, double centerFreq, double span) {
    if(spectrum.isEmpty() || !ui || !ui->plotSpectrum || plotTimer.elapsed() <= 50) {
        return;
    }
    plotTimer.restart();
    QString unit;
    double scale;
    double offset = 0;

    if(centerFreq >= 1e9) {
        unit = "GHz";
        scale = 1e9;
    } else if(centerFreq >= 1e6) {
        unit = "MHz";
        scale = 1e6;
    } else {
        unit = "KHz";
        scale = 1e3;
    }

    QString yAxisName;
    int type = device->getSpectrumType();

    if(type == 0) {
        yAxisName = "Power, dB";
    } else if(type == 2) {
        yAxisName = "Power, dBm";
    } else if(type == 1) {
        yAxisName = "Amplitude, dB";
    }

    int N = spectrum.size();
    double df = sampleRate / N;
    double fmin = centerFreq - sampleRate / 2.0;
    QVector<double> freqs(N);

    for(int i = 0; i < N; i++) {
        freqs[i] = fmin + i * df;
    }

    for(int i = 0; i < freqs.size(); i++) {
        (freqs[i] -= offset) /= scale;
    }

    ui->plotSpectrum->graph(0)->setData(freqs, spectrum);
    ui->plotSpectrum->xAxis->setLabel(QString("Frequency, %1").arg(unit));
    ui->plotSpectrum->yAxis->setLabel(QString("%1").arg(yAxisName));
    ui->plotSpectrum->xAxis->setRange((centerFreq - span / 2) / scale, (centerFreq + span / 2) / scale);

    if(!device->getAutoYRange()) {
        double minY = *std::min_element(spectrum.constBegin(), spectrum.constEnd());
        double maxY = *std::max_element(spectrum.constBegin(), spectrum.constEnd());

        double headroom = 0.15 * (maxY - minY); // 15% сверху
        double curMin = ui->plotSpectrum->yAxis->range().lower;
        double curMax = ui->plotSpectrum->yAxis->range().upper;

        double newMin = curMin + device->getySmoothingAlpha() * (minY - curMin);
        double newMax = curMax + device->getySmoothingAlpha() * ((maxY + headroom) - curMax);

        ui->plotSpectrum->yAxis->setRange(newMin, newMax);
    } else {
        ui->plotSpectrum->yAxis->setRange(-100.0, 0.0);
    }

    ui->plotSpectrum->replot();

}


void MainWindow::on_LNA_Gain_horizontalSlider_valueChanged(int value) {
    ui->LNA_Gain_doubleSpinBox->setValue((double)value);
}


void MainWindow::on_LNA_Gain_doubleSpinBox_valueChanged(double arg1) {
    ui->LNA_Gain_horizontalSlider->setValue((int)arg1);
}


void MainWindow::on_LNA_Gain_pushButton_clicked() {
    device->setLnaGain(ui->LNA_Gain_doubleSpinBox->value());
}


void MainWindow::on_VGA_Gain_horizontalSlider_valueChanged(int value) {
    ui->VGA_Gain_doubleSpinBox->setValue((double)value);
}


void MainWindow::on_VGA_Gain_doubleSpinBox_valueChanged(double arg1) {
    ui->VGA_Gain_horizontalSlider->setValue((int)arg1);
}


void MainWindow::on_VGA_Gain_pushButton_clicked() {
    device->setVgaGain(ui->VGA_Gain_doubleSpinBox->value());
}


void MainWindow::on_Span_pushButton_clicked() {
    device->setSpan(ui->Span_doubleSpinBox->value() * getK(ui->Span_comboBox->currentText()));
}


void MainWindow::on_Span_horizontalSlider_valueChanged(int value) {
    ui->Span_doubleSpinBox->setValue(double(value));
}


void MainWindow::on_Span_doubleSpinBox_valueChanged(double arg1) {
    ui->Span_horizontalSlider->setValue((int)arg1);
}

