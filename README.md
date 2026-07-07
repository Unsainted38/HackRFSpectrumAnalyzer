# HackRFSpectrumAnalyzer

C++/Qt desktop application for spectrum analysis using HackRF SDR.

## Overview

HackRFSpectrumAnalyzer is a desktop spectrum analyzer that connects to a HackRF SDR device, receives IQ samples, performs FFT-based spectrum calculation and displays the result in real time.

## Features

- HackRF device initialization and configuration
- Real-time IQ data receiving
- FFT-based spectrum calculation
- Spectrum visualization with QCustomPlot
- Center frequency, span, RBW/VBW and gain settings
- Power calculation and real-time graph update

## Tech Stack

- C++
- Qt Widgets
- QCustomPlot
- libhackrf
- FFTW3
- Linux

## Architecture

The project is divided into two main parts:

- `HackRFDevice` — device initialization, RX callback handling, IQ data processing and FFT calculation.
- `MainWindow` — GUI controls, plotting and interaction with the device layer.

## What I implemented

- Designed the Qt Widgets GUI for spectrum analyzer control.
- Implemented HackRF device initialization and configuration.
- Implemented IQ data receiving through the libhackrf callback model.
- Added FFT-based spectrum calculation using FFTW3.
- Added real-time spectrum plotting with QCustomPlot.
- Implemented basic power calculation and frequency axis scaling.

## Known limitations

- The project currently uses qmake.
- Some external dependencies must be installed manually.
- Hardware testing requires a HackRF SDR device.
- CMake support is planned.

## Build

Required dependencies:

- Qt
- libhackrf
- FFTW3

Build with qmake:

```bash
qmake hackRF.pro
make
```

## Screenshot

![Spectrum Analyzer UI](docs/images/spectrum_analyzer_ui.png)

The application can be launched without a connected device for UI demonstration.
