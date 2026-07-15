# HackRFSpectrumAnalyzer

C++/Qt desktop spectrum analyzer for HackRF SDR devices with real-time FFT processing and spectrum visualization.

![Spectrum Analyzer UI](docs/images/spectrum_analyzer_ui.png)

## Overview

**HackRFSpectrumAnalyzer** is a desktop application for working with HackRF SDR devices.

The application connects to a HackRF device, receives IQ samples, performs FFT-based spectrum calculation and displays the result in real time using a Qt Widgets interface.

The project demonstrates practical C++/Qt development for hardware-oriented software: device initialization, callback-based data receiving, signal processing, GUI controls and real-time plotting.

## Features

* HackRF device initialization and configuration
* Real-time IQ sample receiving through `libhackrf`
* FFT-based spectrum calculation using `FFTW3`
* Spectrum visualization with `QCustomPlot`
* Center frequency, span, RBW/VBW and gain settings
* Power calculation and frequency axis scaling
* Qt Widgets-based desktop interface
* Separation between device-processing logic and GUI layer

## Tech Stack

* C++17
* Qt Widgets
* Qt Core
* Qt PrintSupport
* CMake
* pkg-config
* QCustomPlot
* libhackrf
* FFTW3
* libusb-1.0
* Linux / Unix-like systems

## Architecture

The project is divided into two main layers:

* `HackRFDevice` — encapsulates HackRF initialization, parameter setup, RX callback handling, IQ sample processing, FFT calculation and spectrum data preparation.
* `MainWindow` — provides GUI controls, connects user actions with the device layer and updates the spectrum plot.

General data flow:

```text
HackRF SDR device
        ↓
libhackrf RX callback
        ↓
IQ sample buffer
        ↓
FFT processing
        ↓
Spectrum / power calculation
        ↓
Qt signal
        ↓
QCustomPlot visualization
```

## Repository Structure

```text
.
├── docs/
│   └── images/
│       └── spectrum_analyzer_ui.png
├── qcustomplot/
│   ├── qcustomplot.cpp
│   └── qcustomplot.h
├── CMakeLists.txt
├── hackrfdevice.cpp
├── hackrfdevice.h
├── main.cpp
├── mainwindow.cpp
├── mainwindow.h
├── mainwindow.ui
└── README.md
```

## Dependencies

Required dependencies:

* Qt 5 or Qt 6
* CMake
* pkg-config
* libhackrf
* FFTW3
* libusb-1.0
* C++ compiler with C++17 support

## Installing Dependencies

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install cmake build-essential pkg-config \
    qtbase5-dev qtbase5-dev-tools \
    libhackrf-dev hackrf \
    libfftw3-dev \
    libusb-1.0-0-dev
```

### macOS

```bash
brew install cmake pkg-config qt hackrf fftw libusb
```

If Qt is installed through Homebrew and CMake cannot find it automatically, pass the Qt prefix manually:

```bash
cmake .. -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"
```

## Build

Clone the repository:

```bash
git clone https://github.com/Unsainted38/HackRFSpectrumAnalyzer.git
cd HackRFSpectrumAnalyzer
```

Create a build directory:

```bash
mkdir -p build
cd build
```

Configure the project:

```bash
cmake ..
```

Build:

```bash
cmake --build .
```

Run the application:

```bash
./HackRFSpectrumAnalyzer
```

The exact binary path may depend on the generator and build directory used by your environment.

## Hardware Requirements

Real spectrum acquisition requires a connected HackRF SDR device.

To check that HackRF is detected by the system, run:

```bash
hackrf_info
```

If the device is not detected, check:

* USB connection
* HackRF drivers and firmware
* user permissions for USB devices on Linux
* installed `libhackrf` package
* whether another application is already using the device

## Running Without Hardware

The source code, architecture and GUI layout can be reviewed without a connected HackRF device.

Real IQ data acquisition, spectrum calculation from live samples and hardware testing require a physical HackRF SDR device connected to the system.

## What I Implemented

* Designed and implemented the Qt Widgets GUI for spectrum analyzer control.
* Implemented HackRF initialization and configuration through `libhackrf`.
* Implemented real-time IQ data receiving using the `libhackrf` callback model.
* Added FFT-based spectrum calculation using `FFTW3`.
* Implemented spectrum plotting and frequency axis scaling with `QCustomPlot`.
* Added basic power calculation and real-time graph updates.
* Structured the application into a device-processing layer and a GUI layer.
* Migrated the project build system from qmake to CMake.

## Build System

The primary build system is **CMake**.

The project uses:

* `CMAKE_AUTOMOC` for Qt meta-object compilation
* `CMAKE_AUTOUIC` for processing `.ui` forms
* `pkg-config` for external native libraries
* C++17 as the required language standard

External libraries are expected to be available through `pkg-config`:

* `libhackrf`
* `fftw3`
* `libusb-1.0`

## Known Limitations

* Hardware acquisition requires a connected HackRF SDR device.
* Some platform-specific dependency installation may require manual setup.
* The project is intended as a desktop engineering tool and portfolio example, not as a full replacement for professional SDR software.
* Current focus is on basic spectrum visualization and hardware interaction rather than advanced SDR processing features.

## Possible Improvements

* Add a mock signal generator for full demo mode without hardware.
* Improve error handling and user-facing diagnostics.
* Add configuration file support for default device parameters.
* Add export of spectrum data to CSV.
* Add GitHub Actions build check.
* Add release builds for Linux.
* Add CMake install rules.
* Add unit tests for DSP/helper logic.
* Add more advanced spectrum processing modes.

## Project Purpose

This project was developed as a practical C++/Qt application for SDR-based spectrum analysis and hardware interaction.

It demonstrates experience with:

* desktop GUI development;
* Qt Widgets applications;
* hardware device APIs;
* callback-based data processing;
* FFT calculations;
* real-time visualization;
* CMake-based C++ project configuration;
* integration of native system libraries.

## License

No license is currently specified.
