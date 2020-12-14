#!/usr/bin/env bash


MEM_CONSTRAINTS=(2G 1G 512M)
RHOS=(0.7 0.8 0.9 0.95)
#NETWORKS=(AgeNet)

MODES=(partial bulk deepeye linear)
# MODES=(bulk)
REPITIONS=100
OUTPUT_PATH=

# NUM_EXPS=$((${#MEM_CONSTRAINTS[@]} * ${#MODES[@]} * ${#NETWORKS[@]}  * $REPITIONS))
NUM_EXPS=$((${#MEM_CONSTRAINTS[@]} * ${#MODES[@]} * ${#RHOS[@]} ))
# 
# echo $NUM_EXPS
IDX=1
# echo $NUM_EXPS


# for i in {1..17}; do
# 	echo "Running variation [${IDX}/${NUM_EXPS}]"
# 	IDX=$((IDX+1))
# done
# # echo "${#NETWORKS[@]}"


BINARY_LOC=./cmake-build-debug
export OPENBLAS_NUM_THREADS=2

CMD_BASE=./Exp_poisson_arrival_rv_nf

CUR_WD=$(eval pwd)

for RHO in ${RHOS[@]}; do
    for BYTE_LIMIT in ${MEM_CONSTRAINTS[@]}; do
        sudo cgcreate -g memory:bulk-inference
        sudo cgset -r memory.limit_in_bytes="${BYTE_LIMIT}" bulk-inference
        sudo cgset -r memory.memsw.limit_in_bytes=8G bulk-inference

        for MODE in ${MODES[@]}; do

            echo "*****************************"
            echo "Running with ${BYTE_LIMIT} LIMIT"
            echo "Running with ${MODE} MODE"
            echo "Running with ${REPITIONS} repitions"
            echo "Running with ${RHO} rho"
            echo "*****************************"
            echo "Running variation [${IDX}/${NUM_EXPS}]"
			IDX=$((IDX+1))
            echo "*****************************"
            CMD="${CMD_BASE} --read-config=../config/pipeline-rv-nf-poisson-arrival.yaml --output-prefix=r${RHO}-${BYTE_LIMIT}-${MODE}- --mem_limit=${BYTE_LIMIT} --num-arrivals=${REPITIONS} --rho=${RHO} --mode=${MODE}"
#              CMD="./RunPipeline ${modeName} sjf_sched_output_${modeName}_${BYTE_LIMIT}.csv ${BYTE_LIMIT}"
            cd $BINARY_LOC
            sudo cgexec -g memory:bulk-inference $CMD
            cd $CUR_WD
        done
        sudo cgdelete -g memory:bulk-inference
    done
done