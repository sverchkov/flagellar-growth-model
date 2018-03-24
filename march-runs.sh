#!/bin/bash

paramsB="-a params/dense_model.txt"
paramsS="-a params/dense_model_slow.txt"

# Initial conditions of length 90 flagellum and 60 ifts
ic="-b ic/many.dat"

# Time limit
t=15000

# Run trajectories
crowding/run ${paramsS} ${ic} ${t} -a crowding_s_trajectory.txt
ift/run ${paramsS} ${ic} ${t} -a original_s_trajectory.txt
crowding/run ${paramsB} ${ic} ${t} -a crowding_b_trajectory.txt
ift/run ${paramsB} ${ic} ${t} -a original_b_trajectory.txt

# Run ensembles
crowding/run ${paramsS} ${ic} ${t} -a crowding_s_ensemble.txt 5000 crowding_s_ensemble.backup
ift/run ${paramsS} ${ic} ${t} -a original_s_ensemble.txt 5000 original_s_ensemble.backup
crowding/run ${paramsB} ${ic} ${t} -a crowding_b_ensemble.txt 5000 crowding_b_ensemble.backup
ift/run ${paramsB} ${ic} ${t} -a original_b_ensemble.txt 5000 oroginal_b_ensemble.backup

