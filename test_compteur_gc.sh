#!/bin/sh

TIME_CMD="/usr/bin/time"
TIME_OPT="-f %e"

rm -r test
mkdir test

rm -r res.dat

echo "gen th time" >> res.dat

for size_g in `awk 'BEGIN {for (i=100; i<=1000000000; i*=10) print i }'`; do
	./aleazard $size_g > ./test/test.txt
	for size_t in `awk 'BEGIN {for (i=1; i<=32; i*=2) print i }'`; do
		temps=$($TIME_CMD "$TIME_OPT" ./compteur-gc ./test/test.txt $size_t 2>&1 > /dev/null)
		echo $size_g $size_t $temps >> res.dat
	done
done

rm -r test