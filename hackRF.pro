QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    hackrfdevice.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot/qcustomplot.cpp

HEADERS += \
    hackrfdevice.h \
    mainwindow.h \
    qcustomplot/qcustomplot.h

FORMS += \
    mainwindow.ui

unix {
    CONFIG += link_pkgconfig

    packagesExist(libhackrf) {
        PKGCONFIG += libhackrf
    } else {
        error("libhackrf не найден. Установи HackRF/libhackrf: macOS: brew install hackrf; Ubuntu/Debian: sudo apt install hackrf")
    }
    packagesExist(fftw3) {
        PKGCONFIG += fftw3
    } else {
        error("fftw3 не найден. Установи fftw3: macOS: brew install fftw; Ubuntu/Debian: sudo apt install fftw")
    }
    packagesExist(libusb-1.0) {
        PKGCONFIG += libusb-1.0
    } else {
        error("libusb-1.0 не найден. Установи libusb-1.0: macOS: brew install libusb-1.0; Ubuntu/Debian: sudo apt install libusb-1.0")
    }
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
