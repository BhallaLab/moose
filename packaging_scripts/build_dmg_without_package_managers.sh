#!/bin/bash

echo "||NOTICE
If you are using this script on MacOSX 10.11.2, be careful about the following
error:
    illegal instruction: 4
I could get everything working fine on MacOSX- 10.8 
"

set -x
set -e

CURRDIR=`pwd`

# Unset any enviroment PYTHONPATH. They can confuse us.
unset PYTHONPATH

echo "|WARN| Unsetting PATH"
unset PATH
export PATH=/usr/bin:/bin:/usr/sbin:/sbin:/usr/X11/bin

APPNAME="Moose"
VERSION="3.0.2"
MAC_NAME=`sw_vers -productVersion`
PKGNAME="${APPNAME}_${VERSION}"

VOLNAME="${PKGNAME}"

### SAFETY
set +e
echo "|| Detaching possibly attached disk"
hdiutil detach /Volumes/${PKGNAME}
set -e

DMGFILELABEL="${PKGNAME}"
THISDIR=`pwd`

# create the temp DMG file
STAGING_DIR=_Install
DMG_TMP="${PKGNAME}-${MAC_NAME}_LOCAL.dmg"
mkdir -p ${STAGING_DIR}

SIZE=1G
echo "|| Creating dmg file of $SIZE size"
if [ ! -f "${DMG_TMP}" ]; then
    hdiutil create -srcfolder "${STAGING_DIR}" -volname "${PKGNAME}" \
        -format UDRW -size $SIZE "${DMG_TMP}"
else
    echo "DMG file $DMG_TMP exists. Mounting ..."
fi

# TODO
# mount it and save the device
DEVICE=$(hdiutil attach -readwrite -noverify "${DMG_TMP}" | \
         egrep '^/dev/' | sed 1q | awk '{print $1}')

############################# EXIT gacefully ################################ 
# Traps etc
# ALWAYS DETACH THE DEVICE BEFORE EXITING...
function detach_device 
{
    hdiutil detach "${DEVICE}"
    exit
}
trap detach_device SIGINT SIGTERM SIGKILL

sleep 1

echo "Install whatever you want now"
PORT_PREFIX="/Volumes/${VOLNAME}"
PYTHONPREFIX=$PORT_PREFIX/pymodules
export PATH=${PORT_PREFIX}/bin:$PATH
(
    WORKDIR=$CURRDIR/_work
    mkdir -p $WORKDIR
    cd $PORT_PREFIX
    ## NOTICE: Try to build using Xcode.

    # 1. Get cmake and install it to cmake/bin 
    CMAKE_BIN=$PORT_PREFIX/cmake/bin/cmake
    if [ -f $CMAKE_BIN ]; then
        echo "|| cmake is available at $CMAKE_BIN "
    else
        echo "No $CMAKE_BIN found. Building one"
        cd $WORKDIR
        curl -O https://cmake.org/files/v3.3/cmake-3.3.2.tar.gz
        tar xvf cmake-3.3.2.tar.gz
        cd cmake-3.3.2 || ls -l
        ./bootstrap
        ./configure --prefix=$PORT_PREFIX/cmake
        make -j3
        make install
        cd $PORT_PREFIX
    fi

    # 2. Install moose-full using cmake.
    MOOSE_PREFIX=$PORT_PREFIX/moose
    export PYTHONPATH=$PYTHONPREFIX/lib/python2.7/site-packages
    if python -c 'import moose'; then
        echo "|STATUS| MOOSE is installed and could be imported"
    else
        cd $WORKDIR
        if [ ! -d moose-full ]; then
            echo "Cloning moose-full"
            git clone --depth 1 https://github.com/BhallaLab/moose-full
        fi
        set -e
        cd moose-full
        mkdir -p _build && cd _build
        $CMAKE_BIN -DCMAKE_INSTALL_PREFIX=$MOOSE_PREFIX .. 
        make -j3
        # Override the install step.
        cd ../moose-core/python 
        python setup.py install --prefix=$PYTHONPREFIX
        python -c 'import moose'
        set +e
        cd $WORKDIR
        cd ..
    fi

    # 3. Let's get Qt4
    QTPREFIX=$PORT_PREFIX/qt4.8
    mkdir -p $QTPREFIX
    (
    mkdir -p $QTPREFIX/bin/
    if [ ! -f $QTPREFIX/bin/qmake ]; then
        echo "|| Trying to install qt"
        cd $WORKDIR
        QTDIR="qt"
        if [ ! -d "$QTDIR" ]; then
            git clone --depth 1 https://github.com/qtproject/qt 
        else
            echo "|| qt is already downloaded."
            #cd qt && git clean -fxd && cd ..
        fi
        cd $QTDIR
        # Delete all license files so qt it does not prompt us to accept
        # license.
        ./configure -opensource -stl -no-qt3support \
            -confirm-license -nomake examples -nomake demos \
            -prefix $QTPREFIX
        make -j3
        make install
    else
        echo "||| Qt seems to be installed. Here are the list of libs"
        ls -lh "$QTPREFIX/lib"
    fi
    )

    # 4. Install SIP 
    SIPPREFIX=$PORT_PREFIX/sip
    mkdir -p $SIPPREFIX
    if python -c 'import sip'; then
        echo "|| SIP is installed"
    else
    (
        cd $WORKDIR
        # Thought of trying this version as it is indicated to work with pyqt4.
        SIPHEAD=4.17
        if [ ! -f sip-$SIPHEAD.tar.gz ] ; then
            rm -f sip-$SIPHEAD.tar.gz
            echo "|| Downloading SIP"
            curl -L -O "https://sourceforge.net/projects/pyqt/files/sip/sip-4.15.5/sip-4.15.5.tar.gz"
        else
            echo "|| SIP is already downloaded and unarchived"
        fi

        tar xvf sip-$SIPHEAD.tar.gz

        cd sip-$SIPHEAD
        echo "|| installing sip"
        python configure.py -b $SIPPREFIX/bin \
            -d $PYTHONPATH \
            -e $SIPPREFIX/include \
            -v $SIPPREFIX/include/sip
        make 
        ( cd siplib && python -c 'import sip' )
        make install
    )
    fi

    # 5. Install PyQt4.
    
    (
        if python -c 'from PyQt4 import pyqtconfig'; then
            echo "|| PyQt4 already installed"
            exit 1
        fi

        cd $WORKDIR
        QTVERSION="4.11.4"
        if [ ! -d PyQt-mac-gpl-$QTVERSION ]; then
            echo "|| Downloading PyQt4"
            curl -L -O "https://www.sourceforge.net/projects/pyqt/files/PyQt4/PyQt-$QTVERSION/PyQt-mac-gpl-$QTVERSION.tar.gz"
            tar xvf PyQt-mac-gpl-$QTVERSION.tar.gz
        else
            echo "|| Already PyQt4 downloaded"
        fi
        cd PyQt-mac-gpl-$QTVERSION
        export PATH=$QTPREFIX/bin:$PATH:$SIPPREFIX/bin
        echo "||| sip: `which sip`"
        #--sip-incdir=$SIPPREFIX/include \
        python configure.py --confirm-license \
            -b $PYTHONPREFIX/bin  \
            -d $PYTHONPATH \
            -v $PYQT4PREFIX/sip
        make  -j3
        make install
    )

    # Install osg
    OSGPREFIX=$PORT_PREFIX/osg
    (
        if [ -f $OSGPREFIX/lib/libosg.dylib ]; then
            echo "||| Looks like osg is installed. Heres the libraries"
            ls $OSGPREFIX/lib
            #exit
        fi
        cd $WORKDIR
        OSG_VERSION=3.2.3
        if [ ! -f OpenSceneGraph-$OSG_VERSION.zip ]; then
            echo "|| Downloading OSG - $OSG_VERSION"
            curl -O http://trac.openscenegraph.org/downloads/developer_releases/OpenSceneGraph-3.2.3.zip
        else
            echo "||| OSG $OSG_VERSION is already downloaded"
        fi
        if [ ! -d OpenSceneGraph-$OSG_VERSION ]; then
            echo "|||| Unzipping "
            unzip OpenSceneGraph-$OSG_VERSION.zip
        fi
        cd OpenSceneGraph-$OSG_VERSION 
        mkdir -p _build
        cd _build
        export QTDIR=$QTPREFIX
        $CMAKE_BIN -DDESIRED_QT_VERSION=4 -DCMAKE_INSTALL_PREFIX=$OSGPREFIX ..
        make -j4
        make install
    )
        
    # Now finally moogli.
    MOOGLI_PREFIX=$PORT_PREFIX/moogli
    (
        export DYLD_FALLBACK_FRAMEWORK_PATH=$QTPREFIX/lib
        export DYLD_LIBRARY_PATH=$OSGPREFIX/lib
        if python -c 'import moogli'; then
            echo "Seems like moogli is already installed"
            exit
        fi

        cd $WORKDIR
        if [ ! -d moogli ]; then
            git clone --depth 1 --branch macosx https://github.com/BhallaLab/moogli
        else
            echo "||| moogli is already downloaded"
        fi
        cd moogli
        export QT_HOME=$QTPREFIX
        export OSG_HOME=$OSGPREFIX
        export PYQT_HOME=$PYTHONPATH/PyQt4
        ( CFLAGS="-m64" CXXFLAGS="" OPT="" ARCHFLAGS="-arch x86_64" python setup.py build )
        if python -c 'import moogli'; then
            CFLAGS="-m64" CXXFLAGS="" OPT="" ARCHFLAGS="-arch x86_64" \
                python setup.py install --prefix=$PYTHONPATH
            # Also copy the _moogli.so, to be sure. distutils behaves oddly.
            cp moogli/core/_moogli.so $PYTHONPATH/moogli/core/_moogli.so
        else
            echo "|| Loading of module is failing."
        fi
    )

    ##||| Install startup scripts.
    MOOSEPATH=${PORT_PREFIX}/lib/python2.7/site-packages
    cat > $PORT_PREFIX/moosegui <<EOF
#!/bin/bash
touch \$HOME/.bash_profile
source \$HOME/.bash_profile
if [[ "\${PYTHONPATH}" == *"${MOOSEPATH}"* ]]; then
    echo "[INFO] PYTHONPATH aleady contains ${MOOSEPATH}"
else
    # Also write to .bash_profile, so that we can use it.
    echo "[INFO] Adding ${MOOSEPATH} to PYTHONPATH"
    echo "export PYTHONPATH=${MOOSEPATH}:\$PYTHONPATH" >> \$HOME/.bash_profile
    source \$HOME/.bash_profile
fi
# make sure that for current runtime, we have correct path.
export PYTHONPATH=${MOOSEPATH}:\$PYTHONPATH
exec ${PORT_PREFIX}/bin/moosegui
EOF
    chmod a+x $PORT_PREFIX/moosegui
)

## Finally detach the device
detach_device
