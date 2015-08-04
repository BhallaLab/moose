#!/usr/bin/env bash
(
    cd ..
    debuild -uc -us | tee $0.log
)
