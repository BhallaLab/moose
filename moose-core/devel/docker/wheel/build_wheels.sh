#!/bin/sh

set -e 

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
NPROC=$(cat /proc/cpuinfo | awk '/^processor/{print $3}' | wc -l)
NUM_WORKERS=$((NPROC-1))

if [ "$TRAVIS" == "true" ]; then
    NUM_WORKERS=2
fi
MAKEOPTS="-j$NUM_WORKERS"

# Place to store wheels.
WHEELHOUSE=${1-$HOME/wheelhouse}
echo "Path to store wheels : $WHEELHOUSE"
mkdir -p $WHEELHOUSE

# Usually docker file copies the source code into the image.
MOOSE_SOURCE_DIR=$(pwd)/moose-core
if [ ! -d $MOOSE_SOURCE_DIR ]; then
    git clone https://github.com/BhallaLab/moose-core --depth 10 
fi

# Try to link statically.
GSL_STATIC_LIBS="/usr/local/lib/libgsl.a;/usr/local/lib/libgslcblas.a"
CMAKE=/usr/bin/cmake3

# Build wheels here.
for PYV in 38 37 36 27; do
    PYDIR=/opt/python/cp${PYV}-cp${PYV}m
    cd $MOOSE_SOURCE_DIR
    echo "Building using $PYDIR in $PYVER"
    PYTHON=$(ls $PYDIR/bin/python?.?)
    if [ "$PYV" -eq 27 ]; then
      $PYTHON -m pip install numpy==1.16
      $PYTHON -m pip install matplotlib==2.2.3
    else
      $PYTHON -m pip install numpy
      $PYTHON -m pip install matplotlib
    fi
    $PYTHON -m pip install wheel
    # $PYTHON setup.py build_ext --with-gsl-static 
    # $PYTHON setup.py bdist_wheel 
    # Don't build numpy. Use wheel.
    export GSL_USE_STATIC_LIBRARIES=1
    $PYTHON -m pip wheel --verbose --no-deps . -w $WHEELHOUSE
    echo "Content of WHEELHOUSE"
    ls -lh $WHEELHOUSE/*.whl
done

# List all wheels.
ls -lh $WHEELHOUSE/*.whl

# now check the wheels.
for whl in $WHEELHOUSE/pymoose*.whl; do
    auditwheel show "$whl"
done

echo "Installing before testing ... "
/opt/python/cp27-cp27m/bin/pip install $WHEELHOUSE/pymoose-$VERSION-py2-none-any.whl
/opt/python/cp36-cp36m/bin/pip install $WHEELHOUSE/pymoose-$VERSION-py3-none-any.whl
for PYV in 36 27; do
    PYDIR=/opt/python/cp${PYV}-cp${PYV}m
    PYTHON=$(ls $PYDIR/bin/python?.?)
    $PYTHON -c 'import moose; print( moose.__version__ )'
done
