#!/usr/bin/env bash
# This script is for launchpad.
(
set -e
set -o xtrace
cd ..
cat > moose.recipe <<EOF
# bzr-builder format 0.3 deb-version {debupstream}+{revno}
lp:moose
EOF
# one can do nesting here.
if [ $# -gt 0 ]; then
    if [[ "$1" == "update" ]]; then
        echo "Fetching repo and creating tar-ball"
        bzr dailydeb --allow-fallback-to-native moose.recipe .. | tee $0.log
    elif [[ "$1" == "pbuilder" ]]; then
        bzr dailydeb --allow-fallback-to-native moose.recipe .. | tee $0.log
        sudo -E pbuilder build ../*.dsc | tee $0.log
        exit
    fi
fi

tarFile=`find .. -type f -maxdepth 1 -name "moose_3.0*.tar.gz"`
echo "Found tarfiles are $tarFile"
if [[ ! $tarFile ]]; then
    echo "I could not file a tar.gz file. Can't continue"
    echo "++ Cleaning previous mess"
    rm -rf ../moose_3.0* ../moose-{*
    echo "++ Let me download a fresh one"
    bzr dailydeb --allow-fallback-to-native moose.recipe .. | tee $0.log
fi

rm -f moose.recipe
echo "Building debian package"
bzr builddeb -- -uc -us | tee $0.log
)
