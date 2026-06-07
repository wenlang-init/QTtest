#!/bin/bash

printComplieCmd(){
    echo -e "\e[1;36m${BASH_SOURCE[1]}->${BASH_SOURCE[0]}(${BASH_LINENO[0]}|${FUNCNAME[1]}->${FUNCNAME[0]})\e[0m"
    echo -e "\e[36m\t$1\e[0m"
}

set -e
#trap "echo '脚本执行出错!' && exit" ERR

rundir=$(pwd)

if [ -d ${rundir}/build ]; then
    printComplieCmd "${rundir}/build exists"
else
    printComplieCmd "mkdir ${rundir}/build"
    mkdir ${rundir}/build
fi

printComplieCmd "cd ${rundir}/build"
cd ${rundir}/build

isrelease=true

if [ -n "$1" ] ; then
    if [ "$1" == "debug" ] ; then
        isrelease=false
    fi
else
    isrelease=true
fi

cpu=$(cat /proc/cpuinfo | grep processor | wc -l)

date1=$(date +"%Y-%m-%d %H:%M:%S")

CROSS_COMPILE_CMAKE=/home/wl/Qt/Tools/CMake/bin/cmake
QTPROFILE=${rundir}/../

CONFIG=" -DCMAKE_PREFIX_PATH=/home/wl/Qt/6.11.1/gcc_64"
CONFIG+=" -DCMAKE_COLOR_DIAGNOSTICS:BOOL=ON"
#CONFIG+=" -DQT_QMAKE_EXECUTABLE:FILEPATH=/home/wl/Qt/6.11.1/gcc_64/bin/qmake"
#CONFIG+=" -DCMAKE_GENERATOR:STRING=Unix Makefiles"

if [ ${isrelease} == false ] ; then
    CONFIG+=" -DCMAKE_BUILD_TYPE:STRING=Debug"
    CONFIG+=" -DQT_ENABLE_QML_DEBUG:BOOL=ON"
else
    CONFIG+=" -DCMAKE_BUILD_TYPE:STRING=Release"
    CONFIG+=" -DQT_ENABLE_QML_DEBUG:BOOL=OFF"
fi

GENERATOR="Unix Makefiles"

# cmake
printComplieCmd "${CROSS_COMPILE_CMAKE} ${CONFIG}  -G \"${GENERATOR}\" -S ${QTPROFILE} -B ${rundir}/build"
${CROSS_COMPILE_CMAKE} ${CONFIG}  -G "${GENERATOR}" -S ${QTPROFILE} -B ${rundir}/build

# make
printComplieCmd "make -f Makefile -j${cpu}"
make -f Makefile -j${cpu}

date2=$(date +"%Y-%m-%d %H:%M:%S")

compiletime=`expr $(date -d "$date2" +%s) - $(date -d "$date1" +%s)`

printComplieCmd "编译时间 ${compiletime}s\n\t开始时间 $date1\n\t结束时间 $date2"
