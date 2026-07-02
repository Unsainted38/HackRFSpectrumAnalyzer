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

## Build

Required dependencies:

- Qt
- libhackrf
- FFTW3

Build with qmake:

```bash
qmake hackRF.pro
make
