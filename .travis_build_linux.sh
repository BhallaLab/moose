#!/usr/bin/env bash
set -o nounset                                  # Treat unset variables as an error
set -e -x
PATH=/usr/bin:/usr/local/bin:$PATH
gbp buildpackage  --git-ignore-branch --git-ignore-new -uc -us -d | tee _gbp.log
pwd 
ls ../*.deb

if [ -n "$TRAVIS" ]; then
    echo "We are on travis"
    sudo dpkg -i ../*.deb
    sudo apt-get install -f 
    python -c 'import moose; print(moose.__file__); print(moose.__version__)'
fi
