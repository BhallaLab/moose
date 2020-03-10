#!/usr/bin/env bash
python3 -m trace --count \
    --ignore-dir=/usr/lib64 \
    --ignore-module=libsbml \
    --ignore-module=numpy \
    --ignore-module=neuroml \
    "$@"

