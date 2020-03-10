#!/bin/bash
set -e 
set -x

BRANCH=$(cat ./BRANCH)
VERSION=3.2.dev$(date +%Y%m%d)

# Just to be sure on homebrew.
export PATH=/usr/local/bin:$PATH

brew update || echo "Failed to update brew"
brew install gsl  || brew upgrade gsl 
brew upgrade python3 || echo "Failed to upgrade python3"
brew upgrade python2 || echo "Failed to upgrade python2"
brew upgrade python || echo "Failed to upgrade python"

# Following are to remove numpy; It is breaking the build on Xcode9.4.
brew uninstall gdal postgis || echo "Failed to uninstall gdal/postgis"
brew uninstall numpy || echo "Failed to uninstall numpy"

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

MOOSE_SOURCE_DIR=`pwd`/moose-core

if [ ! -d $MOOSE_SOURCE_DIR ]; then
    git clone https://github.com/BhallaLab/moose-core -b $BRANCH --depth 10
fi
cd moose-core && git pull
WHEELHOUSE=$HOME/wheelhouse
mkdir -p $WHEELHOUSE
# Current version 0.7.4 seems to be broken with python3.7 .
# See https://travis-ci.org/BhallaLab/deploy/jobs/435219820
sudo /usr/local/bin/python -m pip install delocate virtualenv
sudo /usr/local/bin/python3 -m pip install delocate virtualenv
DELOCATE_WHEEL=/usr/local/bin/delocate-wheel

# Always prefer brew version.
for _py in 3 2; do
    PYTHON=/usr/local/bin/python$_py

    if [ ! -f $PYTHON ]; then
        echo "Not found $PYTHON"
        continue
    fi

    $PYTHON -m pip install setuptools --upgrade --user
    $PYTHON -m pip install wheel --upgrade --user
    $PYTHON -m pip install numpy --upgrade --user
    $PYTHON -m pip install twine  --upgrade  --user

    PLATFORM=$($PYTHON -c "import distutils.util; print(distutils.util.get_platform())")

    ( 
        cd $MOOSE_SOURCE_DIR
	BUILDDIR=_build_$_py
        mkdir -p $BUILDDIR && cd $BUILDDIR
        echo " -- Building wheel for $PLATFORM"
        cmake -DVERSION_MOOSE=$VERSION -DPYTHON_EXECUTABLE=$PYTHON ..

        make -j4
        ( 
            cd python 
            ls *.py
            sed "s/from distutils.*setup/from setuptools import setup/g" \
                setup.cmake.py > setup.wheel.py
            $PYTHON setup.wheel.py bdist_wheel -p $PLATFORM 
            # Now fix the wheel using delocate.
            $DELOCATE_WHEEL -w $WHEELHOUSE -v dist/*.whl
        )

        ls $WHEELHOUSE/pymoose*-py${_py}-*.whl

        # create a virtualenv and test this.
        rm -rf $HOME/Py${_py}
        (
            python3 -m virtualenv -p $PYTHON $HOME/Py${_py}
            source $HOME/Py${_py}/bin/activate
            set +x 
            python -m pip install $WHEELHOUSE/pymoose*-py${_py}-*.whl
            echo "Testing wheel in virtualenv"
            which python
            python --version
            python -c 'import moose; print( moose.__version__ )'
            deactivate
            set -x
        )
    )

    if [ ! -z "$PYPI_PASSWORD" ]; then
        echo "Did you test the wheels? I am uploading anyway ..."
        $PYTHON -m twine upload -u bhallalab -p $PYPI_PASSWORD \
            $HOME/wheelhouse/pymoose*.whl || echo "Failed to upload to PyPi"
    fi
done
