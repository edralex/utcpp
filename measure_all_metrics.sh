#!/bin/bash

prog=$1
name=$2
n=$3

# defaults
if [ -z "$n" ]; then n=10; fi
if [ -z "$name" ]; then name="test"; fi

d=$(date +%Y%m%d_%H%M%S)
out="result_${name}_${d}.txt"

echo "Start test: $name ($d)" > "$out"
echo "File: $prog" >> "$out"

# Size
echo "" >> "$out"
echo "--- Size ---" >> "$out"
bin_path=$(echo "$prog" | awk '{print $1}')
sz=$(stat -c%s "$bin_path" 2>/dev/null || echo "N/A")
echo "$sz bytes" >> "$out"

# Memory
echo "" >> "$out"
echo "--- Memory (Bytes) ---" >> "$out"


valgrind --tool=massif --pages-as-heap=yes --massif-out-file=massif.out "$prog" >/dev/null 2>&1

peak_mem=$(grep "mem_heap_B=" massif.out | sed 's/mem_heap_B=//' | sort -n | tail -n 1)

if [ -z "$peak_mem" ]; then peak_mem=0; fi

echo "$peak_mem" >> "$out"
rm massif.out

echo "Stats Mem:" >> "$out"
echo "Peak: $peak_mem Bytes" >> "$out"


echo "" >> "$out"
echo "--- Time (Average) ---" >> "$out"

avg_time=$(perf stat -r "$n" -- "$prog" 2>&1 | grep "seconds time elapsed" | awk '{print $1}')

echo "$avg_time" >> "$out"
