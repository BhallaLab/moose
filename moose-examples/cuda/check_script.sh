if [ $# != 2 ]; then
	echo "Usage:"
	echo "sh check_script.py <GPU_BUILD_DIR> <CPU_BUILD_DIR>"
	exit
fi

GPU_BUILD_DIR=$1
CPU_BUILD_DIR=$2
#GPU_BUILD_DIR="/home/teja/Work/ncbs/moose-core/build"
#CPU_BUILD_DIR="/home/teja/Work/ncbs/moose-core-static/build"

SCRIPT_PATH="testScript.py"
CPU_DUMP="./cpu/"
GPU_DUMP="./gpu/"

# Removing data of previous runs and making directory
if [ -d "$CPU_DUMP" ]; then
	rm -rf "$CPU_DUMP"
fi
mkdir $CPU_DUMP

if [ -d "$GPU_DUMP" ]; then	
	rm -rf "$GPU_DUMP"
fi
mkdir $GPU_DUMP


# Run the experiment with CPU build
export PYTHONPATH="$CPU_BUILD_DIR/python"
python $SCRIPT_PATH $CPU_DUMP

# Run the experiment with GPU build
export PYTHONPATH="$GPU_BUILD_DIR/python"
python $SCRIPT_PATH $GPU_DUMP


# Check error between cpu and gpu
echo "################### COMPARISION BEGINS GPU/CPU ##############"
echo "Filename, TotalError, AvgError"
for filename in $(ls $CPU_DUMP); do
	python check_error.py "$CPU_DUMP$filename" "$GPU_DUMP$filename"
done
echo "################### COMPARISION ENDS GPU/CPU ##############"

echo "If you see very large non-zero values, then GPU is giving erronous results"
