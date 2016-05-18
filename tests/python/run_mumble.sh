#!/usr/bin/env bash
set -e
echo "Running mumble script $1"
if [ $# -lt 1 ]; then
    echo "Usage: $0 python_script"
    exit
fi
script_name="$1"
export PYTHONPATH=$PYTHONPATH:../../python/
echo "PYTHONPATH $PYTHONPATH"
python $script_name
dot -Teps $script_name.dot > $script_name.eps
#evince $script_name.eps &
