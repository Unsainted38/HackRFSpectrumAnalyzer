#ifndef HACKRFDEVICE_H
#define HACKRFDEVICE_H

#include <QObject>
#include <hackrf.h>
#include <QByteArray>
#include <QMutex>
#include <QVector>
#include <QThread>
#include <complex>
#include <fftw3.h>
#include <QSettings>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <qcustomplot/qcustomplot.h>

enum SpectrumType {
    PowerDB,
    AmplitudeDB,
    PowerDBm
};

struct HackRFDeviceConfig {
    double centerFreq = 100e6;
    double sampleRate = 10e6;
    double rbw = 1e3;
    double span = 1e3;
    int vbw = 10;
    bool autoYRange = false;
    double ySmoothingAlpha = 0.1;
    int lna = 40;
    int vga = 0;
    SpectrumType type = SpectrumType::PowerDB;
};

class HackRFDevice : public QObject {
    Q_OBJECT
public:
    explicit HackRFDevice(QString configPath = "config.ini", QObject *parent = nullptr);
    ~HackRFDevice();

    bool open(); // Открываем девайс
    void close(); // Закрываем девайс
    HackRFDeviceConfig loadConfig(const QString &fileName);
    void setupConfig(HackRFDeviceConfig config);
    QString findConfigFile(const QString &fileName);

    double getSampleRate();
    double getCenterFrequency();
    double getRBW();
    double getVBW();
    //int getVbwAverages();
    bool getAutoYRange();
    double getySmoothingAlpha();
    int getSpectrumType();
    int getLnaGain();
    int getVgaGain();
    double getSpan();
    double getIntegralPower();

    bool setCenterFrequency(double freq); //  Hz
    bool setSampleRate(double sampleRate); //   MHz
    void setSpan(double span);
    void setRBW(double rbw); // Hz
    void setVBW(double vbw); // Кол-во усреднений
    bool setFilter(uint32_t bwHz);
    bool setLnaGain(uint32_t gain); // 0-40 dB step 8 Регулировка усилителя на входе антенны для слабых сигналов, улучшает чувствительность
    // но может привести к перегрузке АЦП, эту платку реально можно сломать безвозвратно)
    bool setVgaGain(uint32_t gain); // 0-62 dB step 2 Регулировка усилителя после фильтра и смесителя для сигнала на базовой частоте
    void setAveragingEnabled(bool en);
    void setAutoYRange(bool flag);
    void setySmoothingAlpha(double value);
    void setCalibrationOffset(double value);
    void setSpectrumType(int type);

    bool start();
    bool start(double centerFreq, double span, double rbw, int averages);
    bool stop();
public slots:
signals:
    void newSamples(const QByteArray &data, double powerDbm);
    void spectrumReady(QVector<double> spectrum, double sampleRate, double centerFreq, double span);
    void centralPowerReady(double);
    void integralPowerReady(double);
    void centerFreqChanged();
    void spanChanged();
    void sampleRateChanged();
    void rbwChanged();
private:
    hackrf_device *m_device;
    QMutex m_mutex;
    bool m_receiving = false;
    double m_sampleRate;
    double m_span;
    double m_minFreq;
    double m_maxFreq;
    double m_centerFreq;
    double m_rbw;
    double m_integralPower = 0;
    int m_fftSize;
    int m_vbwAverages;
    double m_vbwHz;
    bool m_enableAveraging = true;
    bool m_autoYRange = false;
    double m_ySmoothingAlpha = 0.1;
    double m_calibrationOffset = 0.0;
    SpectrumType m_type = SpectrumType::PowerDB;
    int m_LnaGain;
    int m_VgaGain;

    QString m_configPath;


    QVector <std::complex<double>> m_fftBuffer;
    QVector<double> m_accumSpectrum;
    QVector<QVector<double>> m_windowSpectra;
    int m_accumCount;

    static int rxCallback(hackrf_transfer *transfer);
    int handleRx(hackrf_transfer *transfer);

    std::vector<std::complex<double>> convertBufferToIQ(const uint8_t *buffer, size_t length);
    double calculateIntegral(const QVector<double> &spectrum);
    double calculateIntegralPowerDB(const QVector<double> &spectrum);
    double calculateIntegralPowerDBm(const QVector<double> &spectrum);
    double calculateIntegralAmplitudeDB(const QVector<double> &spectrum);
    double calculateCenter(const QVector<double> &spectrum);
    double calculateCenterPowerDB(const QVector<double> &spectrum);
    double calculateCenterPowerDBm(const QVector<double> &spectrum);
    double calculateCenterAmplitudeDB(const QVector<double> &spectrum);
    QVector<double> calculateSpectrum(const std::vector<std::complex<double>> &iq);
    QVector<double> calculateSpectrumAmplitudeDB(const std::vector<std::complex<double>> &iq);
    QVector<double> calculateSpectrumPowerDB(const std::vector<std::complex<double>> &iq);
    QVector<double> calculateSpectrumPowerDBm(const std::vector<std::complex<double>> &iq);
    QVector<double> movingAverageSpectrum(const QVector<double> &spectrum);
    QVector<double> movingAverageSpectrumAmplitudeDB(const QVector<double> &spectrumDB);
    QVector<double> movingAverageSpectrumPowerDB(const QVector<double> &spectrumDB);
    QVector<double> movingAverageSpectrumPowerDBm(const QVector<double> &spectrumDBm);
    QVector<double> averageSpectrum(const QVector<double> &spectrum);
    QVector<double> fftShift(const QVector<double> &data);
    void removeDC(std::vector<std::complex<double> > &buffer);
};

#endif // HACKRFDEVICE_H
