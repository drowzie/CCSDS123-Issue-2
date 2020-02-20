#!/bin/bash
X=512
Y=2000
Z=65
P=15
I="HICO_L2_1.BSQ"
echo "RUNNING TEST FOR $I with parameters X=$X, Y=$Y, Z=$Z, P=$P"
"./main.out" -i $I -x $X -y $Y -z $Z -p $P -f
