#!/bin/bash

enableDebugger=0
for var in "$@"
do
    if [ "$var" == "debug" ] 
    then
        enableDebugger=1
    fi
done

adb logcat -c
adb shell am force-stop ##PACKAGE_NAME##
if [ "$enableDebugger" -eq "1" ]; then
    adb shell touch /sdcard/waitdebugger.txt
    $WN_NDK_TOOL_PATH/ndk-gdb --start --force
else
    adb shell rm /sdcard/waitdebugger.txt
    adb shell am start ##PACKAGE_NAME##/.##ACTIVITY_NAME##
    adb logcat -s ##PACKAGE_NAME##:V | while read line; 
    do 
        echo $line; 
        if echo $line | grep -q 'RETURN'; then 
            adb shell cat /sdcard/stdout.txt
            adb shell rm /sdcard/stdout.txt
            x2=`echo $line | awk '{ print $NF }' | sed 's/\\r//g'`
            ps -ef | grep "adb logcat -s ##PACKAGE_NAME##:V" | grep -v grep | awk '{ print $2 }' | xargs kill 
            exit $x2
        elif echo $line | grep -q '\-\-CRASHED'; then
            adb pull /sdcard/crash
            adb shell am force-stop ##PACKAGE_NAME##
            adb shell rm /sdcard/crash
            adb shell cat /sdcard/stdout.txt
            adb shell rm /sdcard/stdout.txt
            echo "CRASHED"
            sleep 1
            adb shell am force-stop ##PACKAGE_NAME##
            name=Crash
            if [[ -e $name.dmp ]]; then
                i=0
                while [[ -e $name-$i.dmp ]]; do 
                    let i++
                done
                name=$name-$i
            fi
            cat crash > $name.dmp
            rm crash
            ps -ef | grep "adb logcat -s ##PACKAGE_NAME##:V" | grep -v grep | awk '{ print $2 }' | xargs kill 
            exit -1
        fi
    done
fi

