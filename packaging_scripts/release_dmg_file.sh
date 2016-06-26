#!/bin/bash
set -x

BREW_PREFIX=/Volumes/Moose_3.0.2
function test_installation
{
    echo "||||||||| TEST STEP"
    export PYTHONPATH=$BREW_PREFIX/lib/python2.7/site-packages
    python -c 'import moose'
    python -c 'import moogli'
    python -c 'import matplotlib; import numpy'
    python -c 'import networkx as nx'
}

LABEL=Moose_3.0.2

# Not all build dependencies are required at runtime. Lets just disable them.
DMGFILE="$1"
if [ ! $DMGFILE ]; then
    echo "USAGE: $0 dmg_file"
    exit
fi

TEMPDMG="$DMGFILE"_temp.dmg
rm -f $TEMPDMG
cp $DMGFILE $TEMPDMG

echo "|| Detaching dmg file"
hdiutil detach /Volumes/$LABEL

echo "|| Mouting DMG file"
hdiutil attach $TEMPDMG

(
    echo "Removing the temp files"
    cd /Volumes/$LABEL
    du -sh /Volumes/$LABEL
    # One command in each line, else if one fails everyoone fails.
    ./bin/brew uninstall cmake 
    ./bin/brew uninstall gcc
    ./bin/brew uninstall python 
    ./bin/brew uninstall wget 
    ./bin/brew uninstall pkg-config
    ./bin/brew uninstall sqlite

    rm -rf include
    test_installation
    rm -rf Frameworks
    test_installation
    rm -rf Library
    test_installation
    rm -rf etc
    test_installation
    rm -rf var
    test_installation
    (
        cd Cellar/qt/4.8*
        rm -rf *.app
        rm -rf bin
        rm -rf tests
        rm -rf plugins
        rm -rf mkspecs
        rm -rf translations
        rm -rf Frameworks
    )
    test_installation

    # Delete all folders named include and share and doc
    find . -type d -name "include" -print0 | xargs -0 -I d rm -rf d
    test_installation
    find . -type d -name "share" -print0 | xargs -0 -I d rm -rf d
    test_installation
    find . -type d -name "doc" -print0 | xargs -0 -I d rm -rf d
    test_installation
    find . -type d -name "tests" -not -path "*/networkx/*" -print0 | xargs -0 -I d rm -rf d 
    test_installation

    # Remove all *.a
    find . -name "*.a" -exec rm -rf \{} \;
    find . -name "*.html" -exec rm -rf \{} \;
    find . -name "*.pdf" -exec rm -rf \{} \;
    find . -name "*.md" -exec rm -rf \{} \;
    test_installation

    # Adding license.
    curl -O https://gnu.org/licenses/gpl.txt
    mv gpl.txt LICENSE.txt

    du -sh /Volumes/$LABEL
)

# Create another image from this folder.
OUTFILE="$DMGFILE"_RW.dmg
rm -f $OUTFILE
hdiutil create -volname $LABEL -srcfolder /Volumes/$LABEL \
    -ov -format UDRW "$OUTFILE"

ls -lh *.dmg

echo "|| Detaching .."
hdiutil detach /Volumes/$LABEL

echo "|| Compressing "
rm -f "$LABEL"_OSX.dmg
hdiutil convert "$OUTFILE" -format UDBZ -o "$LABEL"_OSX.dmg

