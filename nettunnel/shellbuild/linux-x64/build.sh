#!/bin/bash

printComplieCmd(){
    echo -e "\e[1;36m${BASH_SOURCE[1]}->${BASH_SOURCE[0]}(${BASH_LINENO[0]}|${FUNCNAME[1]}->${FUNCNAME[0]})\e[0m"
    echo -e "\e[36m\t$1\e[0m"
    #echo -e "\e[36m${BASH_SOURCE[1]}->${BASH_SOURCE[0]}(${BASH_LINENO[0]}|${FUNCNAME[1]}->${FUNCNAME[0]}) $1\e[0m"
    #sleep 0.1
    # 数组的0号成员是当前执行的脚本，1号成员是调用当前脚本的脚本 ${BASH_SOURCE[@]}
    # 输出结果为一个数组，第一个是被调用的函数名称，第二个是调用的函数名称 ${FUNCNAME[@]}
    # 数组的0号成员是调用的行，1号成员是当前相对行号 ${BASH_LINENO[@]}
    # 当前行号 ${LINENO}
    # local a=${FUNCNAME[@]}
    # echo "\e[36m$a\e[0m"
}

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

CROSS_COMPILE_QMAKE=/home/wl/Qt/6.8.1/gcc_64/bin/qmake
QTPROFILE=${rundir}/../../NetTunnel.pro

#COMPILE_SPEC="-spec devices/linux-buildroot-g++ CONFIG+=qtquickcompiler"
DEBUGCONFIG="CONFIG+=debug CONFIG+=qml_debug"
RELEASRCONFIG="CONFIG+=qtquickcompiler"

if [ ${isrelease} == false ] ; then
    COMPILE_SPEC="-spec linux-g++ "${DEBUGCONFIG}
else
    COMPILE_SPEC="-spec linux-g++ "${RELEASRCONFIG}
fi

#printComplieCmd "/usr/bin/make -f Makefile distclean"
#/usr/bin/make -f Makefile distclean
#/usr/bin/make -f Makefile clean

# qmake
printComplieCmd "${CROSS_COMPILE_QMAKE} -makefile ${QTPROFILE} ${COMPILE_SPEC}"
${CROSS_COMPILE_QMAKE} -makefile ${QTPROFILE} ${COMPILE_SPEC}

printComplieCmd "/usr/bin/make -f Makefile qmake_all"
/usr/bin/make -f Makefile qmake_all

# make
printComplieCmd "/usr/bin/make -f Makefile -j${cpu}"
/usr/bin/make -f Makefile -j${cpu}

date2=$(date +"%Y-%m-%d %H:%M:%S")

compiletime=`expr $(date -d "$date2" +%s) - $(date -d "$date1" +%s)`

printComplieCmd "编译时间 ${compiletime}s\n\t开始时间 $date1\n\t结束时间 $date2"
