#!/bin/bash
set -e
# This creates a package for pip. For testing purpose
moose_dir=moose-3.0
(
    cd ..
    svn export --force . scripts/$moose_dir
)
(
    cp setup.py $moose_dir/
    cd $moose_dir
    echo "Creating new archive"
    if [ -f dist/$moose_dir.tar.gz ]; then
        rm -f dist/*.tar.gz
    fi
    python setup.py sdist -vv
    echo "Created new archive"
    sudo pip install dist/*.tar.gz --no-clean
    echo "Do the rest in $moose_dir"
)
