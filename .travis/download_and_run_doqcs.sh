#!/usr/bin/env bash

set +e

# Definitions of colors in bash
RESTORE='\033[0m'

RED='\033[00;31m'
GREEN='\033[00;32m'
YELLOW='\033[00;33m'
BLUE='\033[00;34m'
PURPLE='\033[00;35m'
CYAN='\033[00;36m'
LIGHTGRAY='\033[00;37m'

LRED='\033[01;31m'
LGREEN='\033[01;32m'
LYELLOW='\033[01;33m'
LBLUE='\033[01;34m'
LPURPLE='\033[01;35m'
LCYAN='\033[01;36m'
WHITE='\033[01;37m'

function coloredPrint
{
    case $1 in
        "WARN")
            echo -e "[WARN] ${LRED} $2 ${RESTORE} $3"
            ;;
        "INFO")
            echo -e "[INFO] ${LGREEN} $2 ${RESTORE} $3"
            ;;
        "ERROR")
            echo -e "[ERROR] ${RED} $2 ${RESTORE} $3"
            ;;
        "DEBUG")
            echo -e "[DEBUG] ${YELLOW} $2 ${RESTORE} $3"
            ;;
        "STEP")
            echo -e "[STEP] ${BLUE} $2 ${RESTORE} $3"
            ;;
        "TODO")
            echo -e "[TODO] ${CYAN} $2 ${RESTORE} $3"
            ;;
        *)
            echo -e "[$1] $2 $3"
            ;;
    esac
}

coloredPrint "INFO" "Downloading DOQCS database silently"
# Download all models.
wget -q -A"*.g" -r https://doqcs.ncbs.res.in/database/simfile/
coloredPrint "INFO" "Done downloading"

echo '' > __UNCLEAN__DOQCS__
MODELS=`find . -type f -name "*.g"`
for _model in ${MODELS}; do
    echo "===================================================================="
    coloredPrint "INFO" "Running $_model for 1 sec"
    T1=$(date +%s.%N)
    OUT=$(python -c "
import moose
moose.loadModel( '${_model}', '/model', 'gsl' )
moose.reinit( )
moose.start( 1 )
")
    T2=$(date +%s.%N)
    DT=$(echo "$T2-$T1" | bc)
    OUTTRIMMED=`echo $OUT | xargs`
    coloredPrint "INFO" "TOOK $DT seconds to run 1 sec."
    if [[ ! -z $OUTTRIMMED ]]; then
        coloredPrint "WARN" "$_model did not load/run cleanly"
        echo "[] ${_model} \n" >> __UNCLEAN__DOQCS__
        echo "\`\`\`" >> __UNCLEAN__DOQCS__
        echo "$OUTTRIMMED" >> __UNCLEAN__DOQCS__
        echo "\n\`\`\`" >> __UNCLEAN__DOQCS__
        echo $OUTTRIMMED
    else
        coloredPrint "INFO" "$_model loaded just fine. We did NOT check output results"
    fi
done

coloredPrint "INFO" "Following scripts did not run cleanly"
cat __UNCLEAN__DOQCS__
