#include "hackrfdevice.h"
#include <qmath.h>
#include <QDebug>
#include <QTimer>

HackRFDevice::HackRFDevice(QString configPath, QObject *parent)
    : QObject(parent), m_device(nullptr), m_configPath(configPath) {
    if(hackrf_init() != HACKRF_SUCCESS) {
        qWarning() << "HackRF init failed";
    }
}

HackRFDevice::~HackRFDevice() {
    stop();
    close();
    hackrf_exit();
}

bool HackRFDevice::open() {
    QMutexLocker locker(&m_mutex);

    if(m_device) {
        return true;
    }

    int res = hackrf_open(&m_device);

    if(res != HACKRF_SUCCESS) {
        qWarning() << "hackrf_open failed: " << hackrf_error_name((hackrf_error)res);
        return false;
    }

    loadConfig(m_configPath);

    return true;
}

void HackRFDevice::close() {
    QMutexLocker locker(&m_mutex);

    if(m_device) {
        hackrf_close(m_device);
        m_device = nullptr;
    }
}

HackRFDeviceConfig HackRFDevice::loadConfig(const QString &fileName) {
    QString path = findConfigFile(fileName);
    HackRFDeviceConfig cfg;

    if(!QFile::exists(path)) {
        qDebug() << "Config not found, will create default at" << path;
        QSettings settings(path, QSettings::IniFormat);
        settings.beginGroup("HackRFDevice");
        settings.setValue("CenterFreq", cfg.centerFreq);
        settings.setValue("SampleRate", cfg.sampleRate);
        settings.setValue("Span", cfg.span);
        settings.setValue("RBW", cfg.rbw);
        settings.setValue("VBW", cfg.vbw);
        settings.setValue("AutoYRange", cfg.autoYRange);
        settings.setValue("ySmoothingAlpha", cfg.ySmoothingAlpha);
        settings.setValue("LNA", cfg.lna);
        settings.setValue("VGA", cfg.vga);
        settings.setValue("SpectrumType", static_cast<int>(cfg.type));
        settings.endGroup();
        setupConfig(cfg);
        return cfg;
    }

    QSettings settings(path, QSettings::IniFormat);
    settings.beginGroup("HackRFDevice");
    cfg.centerFreq = settings.value("CenterFreq", cfg.centerFreq).toDouble();
    cfg.sampleRate = settings.value("SampleRate", cfg.sampleRate).toDouble();
    cfg.span = settings.value("Span", cfg.span).toDouble();
    cfg.rbw = settings.value("RBW", cfg.rbw).toDouble();
    cfg.vbw = settings.value("VBW", cfg.vbw).toDouble();
    cfg.autoYRange = settings.value("AutoYRange", cfg.autoYRange).toBool();
    cfg.ySmoothingAlpha = settings.value("ySmoothingAlpha", cfg.ySmoothingAlpha).toDouble();
    cfg.lna = settings.value("LNA", cfg.lna).toInt();
    cfg.vga = settings.value("VGA", cfg.vga).toInt();
    cfg.type = static_cast<SpectrumType>(settings.value("SpectrumType", static_cast<int>(cfg.type)).toInt());
    settings.endGroup();

    setupConfig(cfg);

    qDebug() << "Loaded config from:" << path
             << "freq=" << cfg.centerFreq
             << "sampleRate=" << cfg.sampleRate
             << "span=" << cfg.span
             << "rbw=" << cfg.rbw
             << "vbw=" << cfg.vbw
             << "autoyrange" << cfg.autoYRange
             << "ySmoothigAlpha" << cfg.ySmoothingAlpha
             << "LNA gain" << cfg.lna
             << "VGA gain" << cfg.vga
             << "SpectrumType" << static_cast<int>(cfg.type);
    return cfg;
}

void HackRFDevice::setupConfig(HackRFDeviceConfig config) {
    setCenterFrequency(config.centerFreq);
    setSampleRate(config.sampleRate);
    setRBW(config.rbw);
    setVBW(config.vbw);
    setSpan(config.span);
    setAutoYRange(config.autoYRange);
    setySmoothingAlpha(config.ySmoothingAlpha);
    setLnaGain(config.lna);
    setVgaGain(config.vga);
    setSpectrumType(config.type);
}

QString HackRFDevice::findConfigFile(const QString &fileName) {
    QString exePath = QCoreApplication::applicationDirPath() + "/" + fileName;

    if(QFile::exists(exePath)) {
        return exePath;
    }

    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configDir);
    QString userConfig = configDir + "/" + fileName;

    if(QFile::exists(userConfig)) {
        return userConfig;
    }

    return userConfig;
}

double HackRFDevice::getSampleRate() {
    return m_sampleRate;
}

double HackRFDevice::getCenterFrequency() {
    return m_centerFreq;
}

double HackRFDevice::getRBW() {
    return m_rbw;
}

double HackRFDevice::getVBW() {
    return m_vbwHz;
}

//int HackRFDevice::getVbwAverages() {
//    return m_vbwAverages;
//}

bool HackRFDevice::getAutoYRange() {
    return m_autoYRange;
}

double HackRFDevice::getySmoothingAlpha() {
    return m_ySmoothingAlpha;
}

int HackRFDevice::getSpectrumType() {
    return static_cast<int>(m_type);
}

int HackRFDevice::getLnaGain() {
    return m_LnaGain;
}

int HackRFDevice::getVgaGain() {
    return m_VgaGain;
}

double HackRFDevice::getSpan() {
    return m_span;
}

double HackRFDevice::getIntegralPower() {
    return m_integralPower;
}

bool HackRFDevice::setCenterFrequency(double freq) {
    if(!m_device) {
        return false;
    }

    bool ok = hackrf_set_freq(m_device, (uint64_t)freq);

    if(ok != HACKRF_SUCCESS) {
        return false;
    }

    m_centerFreq = freq;
    emit centerFreqChanged();
    qDebug() << "Центральная частота изменена на:" << freq << "Hz";
    return true;
}

bool HackRFDevice::setSampleRate(double sampleRate) {
    if(!m_device) {
        return false;
    }

    int ok = hackrf_set_sample_rate(m_device, sampleRate);

    if(ok != HACKRF_SUCCESS) {
        return false;
    }

    m_sampleRate = sampleRate;
    emit sampleRateChanged();
    qDebug() << "SampleRate изменен на:" << sampleRate << "Hz";
    return true;
}

void HackRFDevice::setSpan(double span) {
    if(span <= 0) {
        return;
    } else if(span > 20000000) {
        m_span = 20000000;
    }

    m_span = span;

    emit spanChanged();
}

void HackRFDevice::setRBW(double rbw) {
    if(rbw <= 0) {
        return;
    }

    // Вычисляем точный размер FFT
    m_fftSize = static_cast<int>(m_sampleRate / rbw);

    m_rbw = m_sampleRate / m_fftSize;
    emit rbwChanged();

    qDebug() << "Запрошен точный RBW =" << rbw << "Hz,"
             << "используется fftSize =" << m_fftSize
             << "=> реальный RBW =" << m_rbw << "Hz";
}

void HackRFDevice::setVBW(double vbw) {
    static const double k = 0.536;
    static const double p = 1.275;

    m_vbwHz = vbw;
    double v1 = pow(k * m_rbw / vbw, p);
    size_t averages = pow(v1 + 1, 1 / p);

    if(averages <= 0) {
        return;
    }

    m_vbwAverages = averages;
    qDebug() << "VBW изменен на:" <<  vbw << "Hz," << averages << "усреднений";

    // Если новое окно меньше текущего, обрезаем старые спектры
    while(m_windowSpectra.size() > m_vbwAverages) {
        const QVector<double> &old = m_windowSpectra.front();

        for(int i = 0; i < m_accumSpectrum.size(); ++i) {
            m_accumSpectrum[i] -= old[i];
        }

        m_windowSpectra.pop_front();
        m_accumCount--;
    }
}

bool HackRFDevice::setFilter(uint32_t bwHz) {
    if(!m_device) {
        return false;
    }

    uint32_t bw = hackrf_compute_baseband_filter_bw_round_down_lt(bwHz);
    return hackrf_set_baseband_filter_bandwidth(m_device, bw) == HACKRF_SUCCESS;
}

bool HackRFDevice::setLnaGain(uint32_t gain) {
    if(!m_device) {
        return false;
    }

    m_LnaGain = gain;
    return hackrf_set_lna_gain(m_device, gain) == HACKRF_SUCCESS;
}

bool HackRFDevice::setVgaGain(uint32_t gain) {
    if(!m_device) {
        return false;
    }

    m_VgaGain = gain;
    return hackrf_set_vga_gain(m_device, gain) == HACKRF_SUCCESS;
}

void HackRFDevice::setAveragingEnabled(bool en) {
    m_enableAveraging = en;
}

void HackRFDevice::setAutoYRange(bool flag) {
    m_autoYRange = flag;
}

void HackRFDevice::setySmoothingAlpha(double value) {
    m_ySmoothingAlpha = value;
}

void HackRFDevice::setCalibrationOffset(double value) {
    m_calibrationOffset = value;
}

void HackRFDevice::setSpectrumType(int type) {
    m_type = static_cast<SpectrumType>(type);
}

bool HackRFDevice::start() {
    if(!m_device) {
        return false;
    }

    if(m_receiving) {
        return true;
    }

    int res = hackrf_start_rx(m_device, &HackRFDevice::rxCallback, this);

    if(res == HACKRF_SUCCESS) {
        m_receiving = true;
        qDebug() << "Старт анализатора:"
                 << "Центр =" << m_centerFreq
                 << "Hz, SampleRate =" << m_sampleRate
                 << "Hz, FFT =" << m_fftSize
                 << "точек, VBW =" << m_vbwAverages
                 << "усреднений, реальный RBW =" << (m_sampleRate / m_fftSize) << "Hz";
        return true;
    }



    qWarning() << "hackrf_start_rx failed: " << hackrf_error_name((hackrf_error)res);
    return false;
}

bool HackRFDevice::start(double centerFreq, double span, double rbw, int averages) {
    if(!m_device) {
        return false;
    }

    if(m_receiving) {
        return true;
    }

    m_centerFreq = centerFreq;
    m_sampleRate = span;
    m_vbwAverages = averages;

    // --- Sample rate ---
    if(hackrf_set_sample_rate(m_device, m_sampleRate) != HACKRF_SUCCESS) {
        qWarning() << "Ошибка установки sample rate =" << m_sampleRate;
        return false;
    }

    // --- Центральная частота ---
    if(hackrf_set_freq(m_device, (uint64_t)m_centerFreq) != HACKRF_SUCCESS) {
        qWarning() << "Ошибка установки частоты =" << m_centerFreq;
        return false;
    }

    // --- FFTsize для точного RBW ---
    if(rbw > 0) {
        setRBW(rbw);
    }

    qDebug() << "Старт анализатора:"
             << "Центр =" << m_centerFreq
             << "Hz, SampleRate =" << m_sampleRate
             << "Hz, FFT =" << m_fftSize
             << "точек, VBW =" << m_vbwAverages
             << "усреднений, реальный RBW =" << (m_sampleRate / m_fftSize) << "Hz";

    // --- Запуск приёма ---
    if(hackrf_start_rx(m_device, &HackRFDevice::rxCallback, this) != HACKRF_SUCCESS) {
        qWarning() << "Ошибка запуска приёма";
        return false;
    }

    m_receiving = true;
    return true;
}

bool HackRFDevice::stop() {
    if(!m_device || !m_receiving) {
        return false;
    }

    hackrf_stop_rx(m_device);
    m_receiving = false;

    return true;
}

int HackRFDevice::rxCallback(hackrf_transfer *transfer) {
    HackRFDevice *self = reinterpret_cast<HackRFDevice *>(transfer->rx_ctx);
    return self ? self->handleRx(transfer) : 0;
}

int HackRFDevice::handleRx(hackrf_transfer *transfer) {
    auto iq = convertBufferToIQ(transfer->buffer, transfer->buffer_length);
    removeDC(iq);
    QVector<double> spectrum = calculateSpectrum(iq);
    QVector<double> shiftedSpectrum = fftShift(spectrum);
    QVector<double> avgSpectrum = movingAverageSpectrum(shiftedSpectrum);
    double centerPower = calculateCenter(avgSpectrum);
    double integralPower = m_integralPower = calculateIntegral(avgSpectrum);


    if(!avgSpectrum.isEmpty()) {
        emit spectrumReady(avgSpectrum, m_sampleRate, m_centerFreq, m_span);
    }

    emit centralPowerReady(centerPower);
    emit integralPowerReady(integralPower);

    return 0;
}

std::vector<std::complex<double>> HackRFDevice::convertBufferToIQ(const uint8_t *buffer, size_t length) {
    size_t count = length / 2;
    std::vector<std::complex<double>> iq(count);

    for(size_t i = 0; i < count; ++i) {
        // смещаем в центр и нормализуем в -1..1
        double I = (static_cast<int>(buffer[2 * i]) - 128) / 128.0;
        double Q = (static_cast<int>(buffer[2 * i + 1]) - 128) / 128.0;
        iq[i] = std::complex<double>(I, Q);
    }

    return iq;
}

double HackRFDevice::calculateIntegral(const QVector<double> &spectrum) {
    if(m_type == SpectrumType::PowerDB) {
        return calculateIntegralPowerDB(spectrum);
    } else if(m_type == SpectrumType::AmplitudeDB) {
        return calculateIntegralAmplitudeDB(spectrum);
    } else if(m_type == SpectrumType::PowerDBm) {
        return calculateIntegralPowerDBm(spectrum);
    }
    return 0.0;
}

double HackRFDevice::calculateIntegralPowerDB(const QVector<double> &spectrum) {
    int N = spectrum.size();
    double span = m_span;
    double rbw = m_rbw;
    double centerFreq = m_centerFreq;
    double sampleRate = m_sampleRate;

    if(N == 0 || span <= 0) {
        return 0.0;
    }

    QVector<double> spectrumLinear(N);

    for(int i = 0; i < N; i++) {
        spectrumLinear[i] = std::pow(10.0, spectrum[i] / 10.0);
    }

    double fStart = centerFreq - span / 2.0;
    double fEnd = centerFreq + span / 2.0;
    int startBin = std::max(0, int((fStart - (centerFreq - sampleRate / 2.0)) / rbw));
    int endBin = std::min(N - 1, int((fEnd - (centerFreq - sampleRate / 2.0)) / rbw));
    double integralPower = 0.0;

    for(int i = startBin; i <= endBin; i++) {
        integralPower += spectrumLinear[i] * rbw;
    }

    return 10.0 * log10(integralPower + 1e-12);
}

double HackRFDevice::calculateIntegralPowerDBm(const QVector<double> &spectrum) {
    int N = spectrum.size();
    double span = m_span;
    double rbw = m_rbw;
    double centerFreq = m_centerFreq;
    double sampleRate = m_sampleRate;

    if(N == 0 || span <= 0) {
        return 0.0;
    }

    QVector<double> spectrumLinear(N);

    for(int i = 0; i < N; i++) {
        spectrumLinear[i] = std::pow(10.0, spectrum[i] / 10.0);
    }

    double fStart = centerFreq - span / 2.0;
    double fEnd = centerFreq + span / 2.0;
    int startBin = std::max(0, int((fStart - (centerFreq - sampleRate / 2.0)) / rbw));
    int endBin = std::min(N - 1, int((fEnd - (centerFreq - sampleRate / 2.0)) / rbw));
    double integralPower = 0.0;

    for(int i = startBin; i <= endBin; i++) {
        integralPower += spectrumLinear[i] * rbw;
    }

    return 10.0 * log10(integralPower + 1e-12);
}

double HackRFDevice::calculateIntegralAmplitudeDB(const QVector<double> &spectrum) {
    int N = spectrum.size();
    double span = m_span;
    double rbw = m_rbw;
    double centerFreq = m_centerFreq;
    double sampleRate = m_sampleRate;

    if(N == 0 || span <= 0) {
        return 0.0;
    }

    QVector<double> spectrumLinear(N);

    for(int i = 0; i < N; i++) {
        spectrumLinear[i] = std::pow(10.0, spectrum[i] / 10.0);
    }

    double fStart = centerFreq - span / 2.0;
    double fEnd = centerFreq + span / 2.0;
    int startBin = std::max(0, int((fStart - (centerFreq - sampleRate / 2.0)) / rbw));
    int endBin = std::min(N - 1, int((fEnd - (centerFreq - sampleRate / 2.0)) / rbw));
    double integralPower = 0.0;

    for(int i = startBin; i <= endBin; i++) {
        integralPower += spectrumLinear[i] * rbw;
    }

    return 20.0 * log10(integralPower + 1e-12);
}

double HackRFDevice::calculateCenter(const QVector<double> &spectrum) {
    if(m_type == SpectrumType::PowerDB) {
        return calculateCenterPowerDB(spectrum);
    } else if(m_type == SpectrumType::AmplitudeDB) {
        return calculateCenterAmplitudeDB(spectrum);
    } else if(m_type == SpectrumType::PowerDBm) {
        return calculateCenterPowerDBm(spectrum);
    }
    return 0.0;
}
double HackRFDevice::calculateCenterPowerDB(const QVector<double> &spectrum) {
    if(spectrum.isEmpty()) {
        return -1000.0;    // ошибка/нет данных
    }

    int N = spectrum.size();
    int centerIndex = N / 2;

    int span = 5; // сколько бинов влево/вправо захватывать

    QVector<double> spectrumLinear(N);

    for(int i = 0; i < N; i++) {
        spectrumLinear[i] = std::pow(10.0, spectrum[i] / 10.0);
    }

    // перевод обратно в dB
    double maxLinear = *std::max_element(spectrumLinear.begin() + centerIndex - span, spectrumLinear.begin() + centerIndex + span);
    return 10 * log10(maxLinear + 1e-12);
}

double HackRFDevice::calculateCenterPowerDBm(const QVector<double> &spectrum) {
    if(spectrum.isEmpty()) {
        return -1000.0;    // ошибка/нет данных
    }

    int N = spectrum.size();
    int centerIndex = N / 2;

    int span = 5; // сколько бинов влево/вправо захватывать

    QVector<double> spectrumLinear(N);

    for(int i = 0; i < N; i++) {
        spectrumLinear[i] = std::pow(10.0, spectrum[i] / 10.0);
    }

    // перевод обратно в dB
    double maxLinear = *std::max_element(spectrumLinear.begin() + centerIndex - span, spectrumLinear.begin() + centerIndex + span);
    return 10 * log10(maxLinear + 1e-12);
}

double HackRFDevice::calculateCenterAmplitudeDB(const QVector<double> &spectrum) {
    if(spectrum.isEmpty()) {
        return -1000.0;    // ошибка/нет данных
    }

    int N = spectrum.size();
    int centerIndex = N / 2;

    int span = 5; // сколько бинов влево/вправо захватывать

    QVector<double> spectrumLinear(N);

    for(int i = 0; i < N; i++) {
        spectrumLinear[i] = std::pow(10.0, spectrum[i] / 20.0);
    }

    // перевод обратно в dB
    double maxLinear = *std::max_element(spectrumLinear.begin() + centerIndex - span, spectrumLinear.begin() + centerIndex + span);
    return 20 * log10(maxLinear + 1e-12);
}

QVector<double> HackRFDevice::calculateSpectrum(const std::vector<std::complex<double> > &iq) {
    if(m_type == SpectrumType::AmplitudeDB) {
        return calculateSpectrumAmplitudeDB(iq);
    } else if(m_type == SpectrumType::PowerDB) {
        return calculateSpectrumPowerDB(iq);
    } else if(m_type == SpectrumType::PowerDBm) {
        return calculateSpectrumPowerDBm(iq);
    }
    return QVector<double>();
}

QVector<double> HackRFDevice::calculateSpectrumAmplitudeDB(const std::vector<std::complex<double>> &iq) {
    size_t N = m_fftSize;
    fftw_complex *in = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex *out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * N);

    // Копируем данные, заполняем нулями если iq.size() < N
    for(size_t i = 0; i < N; ++i) {
        if(i < iq.size()) {
            in[i][0] = iq[i].real();
            in[i][1] = iq[i].imag();
        } else {
            in[i][0] = 0;
            in[i][1] = 0;
        }
    }

    fftw_plan plan = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);

    QVector<double> spectrum(N);

    for(size_t i = 0; i < N; ++i) {
        std::complex<double> c(out[i][0], out[i][1]);
        spectrum[i] = 20.0 * log10(std::abs(c) / N + 1e-12);   // амплитуда, нормализация
    }

    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);

    return spectrum;
}

QVector<double> HackRFDevice::calculateSpectrumPowerDB(const std::vector<std::complex<double> > &iq) {
    size_t N = m_fftSize;
    fftw_complex *in = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex *out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * N);

    // Копируем данные, заполняем нулями если iq.size() < N
    for(size_t i = 0; i < N; ++i) {
        if(i < iq.size()) {
            in[i][0] = iq[i].real();
            in[i][1] = iq[i].imag();
        } else {
            in[i][0] = 0;
            in[i][1] = 0;
        }
    }

    fftw_plan plan = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);

    QVector<double> spectrum(N);

    for(size_t i = 0; i < N; ++i) {
        std::complex<double> c(out[i][0], out[i][1]);
        spectrum[i] = 10.0 * log10(std::norm(c) / (N * N) + 1e-12);   // мощность в дБ, нормализация
    }

    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);

    return spectrum;
}

QVector<double> HackRFDevice::calculateSpectrumPowerDBm(const std::vector<std::complex<double> > &iq) {
    size_t N = m_fftSize;
    fftw_complex *in = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex *out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * N);

    // Копируем данные, заполняем нулями если iq.size() < N
    for(size_t i = 0; i < N; ++i) {
        if(i < iq.size()) {
            in[i][0] = iq[i].real();
            in[i][1] = iq[i].imag();
        } else {
            in[i][0] = 0;
            in[i][1] = 0;
        }
    }

    fftw_plan plan = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);

    QVector<double> spectrum(N);

    const double R = 50.0;

    for(size_t i = 0; i < N; ++i) {
        std::complex<double> c(out[i][0], out[i][1]);
        spectrum[i] = 10.0 * log10(std::norm(c) / (N * N * R) * 1000.0 + 1e-12);   // мощность в дБм, нормализация
    }

    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);

    return spectrum;
}

QVector<double> HackRFDevice::movingAverageSpectrum(const QVector<double> &spectrum) {
    if(m_type == SpectrumType::AmplitudeDB) {
        return movingAverageSpectrumAmplitudeDB(spectrum);
    } else if(m_type == SpectrumType::PowerDB) {
        return movingAverageSpectrumPowerDB(spectrum);
    } else if(m_type == SpectrumType::PowerDBm) {
        return movingAverageSpectrumPowerDBm(spectrum);
    }
    return QVector<double>();
}

QVector<double> HackRFDevice::movingAverageSpectrumAmplitudeDB(const QVector<double> &spectrumDB) {
    int N = spectrumDB.size();

    // Инициализация
    if(m_accumSpectrum.size() != N) {
        m_accumSpectrum.resize(N);
        m_accumSpectrum.fill(0.0);
        m_windowSpectra.clear();
        m_accumCount = 0;
    }

    // Перевод в линейную амплитуду
    QVector<double> spectrumLinear(N);

    for(int i = 0; i < N; ++i) {
        spectrumLinear[i] = pow(10.0, spectrumDB[i] / 20.0);
    }

    // Добавляем новый спектр
    for(int i = 0; i < N; ++i) {
        m_accumSpectrum[i] += spectrumLinear[i];
    }

    m_windowSpectra.push_back(spectrumLinear);
    m_accumCount++;

    // Удаляем старые спектры при превышении окна
    while(m_windowSpectra.size() > m_vbwAverages) {
        const QVector<double> &old = m_windowSpectra.front();

        for(int i = 0; i < N; ++i) {
            m_accumSpectrum[i] -= old[i];
        }

        m_windowSpectra.pop_front();
        m_accumCount--;
    }

    // Усреднение в линейке
    QVector<double> avgLinear(N);

    for(int i = 0; i < N; ++i) {
        avgLinear[i] = m_accumSpectrum[i] / m_accumCount;
    }

    // Перевод обратно в dB
    QVector<double> avgDB(N);

    for(int i = 0; i < N; ++i) {
        avgDB[i] = 20.0 * log10(avgLinear[i] + 1e-12);
    }

    return avgDB;
}

QVector<double> HackRFDevice::movingAverageSpectrumPowerDB(const QVector<double> &spectrumDB) {
    int N = spectrumDB.size();

    // Инициализация
    if(m_accumSpectrum.size() != N) {
        m_accumSpectrum.resize(N);
        m_accumSpectrum.fill(0.0);
        m_windowSpectra.clear();
        m_accumCount = 0;
    }

    // Перевод в линейную мощность
    QVector<double> spectrumLinear(N);

    for(int i = 0; i < N; ++i) {
        spectrumLinear[i] = pow(10.0, spectrumDB[i] / 10.0);
    }

    // Добавляем новый спектр
    for(int i = 0; i < N; ++i) {
        m_accumSpectrum[i] += spectrumLinear[i];
    }

    m_windowSpectra.push_back(spectrumLinear);
    m_accumCount++;

    // Удаляем старые спектры при превышении окна
    while(m_windowSpectra.size() > m_vbwAverages) {
        const QVector<double> &old = m_windowSpectra.front();

        for(int i = 0; i < N; ++i) {
            m_accumSpectrum[i] -= old[i];
        }

        m_windowSpectra.pop_front();
        m_accumCount--;
    }

    // Усреднение в линейке
    QVector<double> avgLinear(N);

    for(int i = 0; i < N; ++i) {
        avgLinear[i] = m_accumSpectrum[i] / m_accumCount;
    }

    // Перевод обратно в dB
    QVector<double> avgDB(N);

    for(int i = 0; i < N; ++i) {
        avgDB[i] = 10.0 * log10(avgLinear[i] + 1e-12);
    }

    return avgDB;
}

QVector<double> HackRFDevice::movingAverageSpectrumPowerDBm(const QVector<double> &spectrumDBm) {
    int N = spectrumDBm.size();

    // Инициализация
    if(m_accumSpectrum.size() != N) {
        m_accumSpectrum.resize(N);
        m_accumSpectrum.fill(0.0);
        m_windowSpectra.clear();
        m_accumCount = 0;
    }

    // Перевод в линейную мощность
    QVector<double> spectrumLinear(N);

    for(int i = 0; i < N; ++i) {
        spectrumLinear[i] = pow(10.0, spectrumDBm[i] / 10.0);
    }

    // Добавляем новый спектр
    for(int i = 0; i < N; ++i) {
        m_accumSpectrum[i] += spectrumLinear[i];
    }

    m_windowSpectra.push_back(spectrumLinear);
    m_accumCount++;

    // Удаляем старые спектры при превышении окна
    while(m_windowSpectra.size() > m_vbwAverages) {
        const QVector<double> &old = m_windowSpectra.front();

        for(int i = 0; i < N; ++i) {
            m_accumSpectrum[i] -= old[i];
        }

        m_windowSpectra.pop_front();
        m_accumCount--;
    }

    // Усреднение в линейке
    QVector<double> avgLinear(N);

    for(int i = 0; i < N; ++i) {
        avgLinear[i] = m_accumSpectrum[i] / m_accumCount;
    }

    // Перевод обратно в dB
    QVector<double> avgDB(N);

    for(int i = 0; i < N; ++i) {
        avgDB[i] = 10.0 * log10(avgLinear[i] + 1e-12);
    }

    return avgDB;
}

QVector<double> HackRFDevice::averageSpectrum(const QVector<double> &spectrum) {
    int fftSize = spectrum.size();

    if(m_accumSpectrum.size() != fftSize) {
        m_accumSpectrum.resize(fftSize);
        m_accumSpectrum.fill(0.0);
        m_accumCount = 0;
    }

    for(int i = 0; i < fftSize; ++i) {
        m_accumSpectrum[i] += spectrum[i];  // суммируем амплитуды
    }

    m_accumCount++;

    QVector<double> avg(fftSize);

    if(m_enableAveraging && m_accumCount >= m_vbwAverages) {
        for(int i = 0; i < fftSize; ++i) {
            avg[i] = 20 * log10(m_accumSpectrum[i] / m_accumCount + 1e-12); // амплитуда → dB
        }

        m_accumSpectrum.fill(0.0);
        m_accumCount = 0;
    } else {
        for(int i = 0; i < fftSize; ++i) {
            avg[i] = 20 * log10(spectrum[i] + 1e-12); // без усреднения
        }
    }

    return avg;
}

QVector<double> HackRFDevice::fftShift(const QVector<double> &data) {
    QVector<double> shifted = data;
    int N = shifted.size();
    std::rotate(shifted.begin(),
                shifted.begin() + N / 2,
                shifted.end());
    return shifted;
}

void HackRFDevice::removeDC(std::vector<std::complex<double> > &buffer) {
    if(buffer.empty()) {
        return;
    }

    std::complex<double> mean(0, 0);

    for(auto &v : buffer) {
        mean += v;
    }

    mean /= static_cast<double>(buffer.size());

    for(auto &v : buffer) {
        v -= mean;
    }
}
