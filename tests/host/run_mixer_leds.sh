#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
# fm1 fork: run after ./build.sh or host/build_host.sh has generated build/gen assets.
set -e
cd "$(dirname "$0")/../.."
mkdir -p build/host
cc -O2 -ffp-contract=off -std=c99 -D_POSIX_C_SOURCE=200809L -DX0X_HOST \
    -Ifirmware/src -Ifirmware/src/dsp -Ibuild/gen -o build/host/mixer_led_test \
    tests/host/mixer_led_test.c firmware/src/dsp/drum909.c firmware/src/dsp/drum808.c \
    firmware/src/dsp/bass303.c firmware/src/dsp/breaks.c firmware/src/dsp/fxbus.c \
    firmware/src/dsp/master.c firmware/src/seq/sequencer.c firmware/src/seq/tb3po.c \
    firmware/src/seq/pattern.c firmware/src/seq/motion.c firmware/src/app/engine.c -lm
build/host/mixer_led_test
