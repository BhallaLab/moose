#!/usr/bin/env bash
brew tap BhallaLab/moose
brew install --HEAD moose-nightly
python -m pip install networkx python-libsbml pyNeuroML  matplotlib
python -c 'import moose; print( moose.__version__ )'
