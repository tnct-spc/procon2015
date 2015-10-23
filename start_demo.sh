#!/bin/sh

export BUILDDIR="${HOME}/projects/build-procon2015-unknown-Release"
export PROBDIR="${HOME}/problems"
export PROBLEMS=${PROBDIR}/*.txt
export LD_LIBRARY_PATH="${BUILDDIR}/takao:${BUILDDIR}/tengu:${BUILDDIR}/muen_zuka"
export SERVER="${BUILDDIR}/outside_server/outside_server"
export SLAVE="${BUILDDIR}/slave/slave"
export RUNSEC="300"
export ENABLE_ALGO="0 1 2 4 9"

while true; do
    for fn in ${PROBLEMS}; do
        timeout -sKILL ${RUNSEC} ${SERVER} $fn &
        sleep 5;
        for algo in ${ENABLE_ALGO}; do
            timeout -sKILL `expr ${RUNSEC} - 5` ${SLAVE} $algo player$algo &
        done
        sleep `expr ${RUNSEC} - 5`
    done
done

