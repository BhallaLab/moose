#!/usr/bin/env bash
set -o nounset                                  # Treat unset variables as an error
set -e -x
PATH=/usr/bin:/usr/local/bin:$PATH
gbp buildpackage  --git-ignore-branch --git-ignore-new -uc -us -d | tee _gbp.log
pwd 
