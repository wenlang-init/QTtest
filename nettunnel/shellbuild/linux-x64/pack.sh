#!/bin/bash

exe=$1
des=$2
#des=$(pwd)

printComplieCmd(){
    #echo -e "$# $0 $* alice: $@"
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

printComplieCmdNoFFL(){
    echo -e "\e[36m$1\e[0m"
}

CROSS_COMPILE=
#deplist=$(${CROSS_COMPILE}readelf -a ${exe} |grep "Shared")
#deplist=`${CROSS_COMPILE}objdump -x ${exe} |grep "NEEDED"|awk '{
#deplist=`${CROSS_COMPILE}objdump -x ${exe} |grep "\.so"|awk '{
#if(match($3,"libQt"))
#{
#        printf("%s "),$3
#}
#else if(match($3,"libqt"))
#{
#        printf("%s "),$3
#}
#else
#{
#        printf("%s "),$3
#}
#}'`

istrue=0
if [ -n "$1" ] ; then
    if [ -n "$2" ] ; then
        istrue=1
    fi
fi

if [ ${istrue} -eq 0 ] ; then
    printComplieCmd "参数错误:$0 appName targetPath"
    exit 0
fi

deplist=$(ldd $exe | awk '{if (match($3,"/")){printf("%s "),$3}}')
undeplist=$(ldd $exe | awk '{if (!match($3,"/")){printf("%s "),$0}}')
printComplieCmd "no copy:${undeplist}"

printComplieCmd "copy:${deplist}"

printComplieCmd "copy library:"
#echo $deplist
libcount=0
for var in $deplist
do
#	echo "lib file:$var"
#        printComplieCmd "cp $var $des -v"
        printComplieCmdNoFFL "cp $var $des"
        cp $var $des
        #cp $var $des -v
	libcount=$((libcount+1))
done
printComplieCmd "copy library file count:$libcount"
