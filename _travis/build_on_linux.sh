#!/bin/bash -
#===============================================================================
#
#          FILE: build_on_travis.sh
#
#         USAGE: ./build_on_travis.sh
#
#   DESCRIPTION: 
#
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Dilawar Singh (), dilawars@ncbs.res.in
#  ORGANIZATION: NCBS Bangalore
#       CREATED: Tuesday 27 June 2017 01:55:11  IST
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error
set -e

(
    gbp buildpackage  --git-ignore-branch --git-ignore-new -uc -us
    cd __moose-core_build && ctest --output-on-failure && cd ..
    sudo dpkg -D=2 -i ../moose*.deb
    cd ~ && /usr/bin/python -c \
        "import moose; print moose.version(); print( moose.__file__ ); moose.test( timeout = 10 )"

    # Now launch gui, terminate is after 3 seconds.
    timeout 3 moosegui

)
