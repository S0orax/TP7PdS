#!/usr/bin/sh

# dd if=/dev/urandom of=/tmp/alea bs=1048576 count=128
echo 'Non trie'
for size in `awk 'BEGIN {for (i=1; i<=5; i++) print i }'`; do
	dd if=/dev/urandom of=/tmp/alea bs=1048576 count=100
	echo $size
	/usr/bin/time -f %e ./tri-rapide -n $size -c /tmp/alea
done

echo 'trie'
for size in `awk 'BEGIN {for (i=1; i<=5; i++) print i }'`; do
	echo $size
	/usr/bin/time -f %e ./tri-rapide -n $size -c /tmp/alea
done