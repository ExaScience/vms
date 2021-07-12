#!/bin/bash
#SBATCH --job-name=af_predict
#SBATCH --error=af_predict-%j.err
#SBATCH --output=af_predict-%j.out
#SBATCH --partition=euroexa
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --exclusive
#SBATCH --gres=gpu:1


BASEDIR=/home/tvandera/euroexa/smurff/vms/af_cpp

cd $BASEDIR

echo "Measuring idle power"
for i in {1..10}
do
	echo "Sleep 1..."
	srun sleep 1
	echo "Power: "
	srun sudo /root/scripts/power-measuremets/ipmi-get-node-power.sh
done

echo
echo "Measuring GPU power (cuda)"
for i in {1..10}
do
	srun ./af_predict --repeat 1 --backend cuda
	echo "Power: "
	srun sudo /root/scripts/power-measuremets/ipmi-get-node-power.sh
done

echo
echo "Measuring CPU power (Eigen/MKL)"
for i in {1..10}
do
	srun ./af_predict --repeat 1 --backend eigen
	echo "Power: "
	srun sudo /root/scripts/power-measuremets/ipmi-get-node-power.sh
done

