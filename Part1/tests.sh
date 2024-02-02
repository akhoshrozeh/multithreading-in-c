#!/bin/bash

# restart the tests
rm -f lab2_add.csv
touch lab2_add.csv
rm -f lab2_list.csv
touch lab2_list.csv

threads=(1 2 4 8 12)
iterations=(1 10 100 1000 10000 100000)
list_iterations=(1 2 4 8 16 32)
list_threads=(1 2 4 8 12)

### ADD TESTING ###

# add-none ... no yield, no synchronization
for i in "${threads[@]}"; do
    for j in "${iterations[@]}"; do
        ./lab2_add --threads=$i --iterations=$j >> lab2_add.csv
    done
done

# add-m ... no yield, mutex synchronization
for i in "${threads[@]}"; do
    for j in "${iterations[@]}"; do
        ./lab2_add --threads=$i --iterations=$j --sync=m >> lab2_add.csv
    done
done

# add-s ... no yield, spin-lock synchronization
for i in "${threads[@]}"; do
    for j in "${iterations[@]}"; do
        ./lab2_add --threads=$i --iterations=$j --sync=s >> lab2_add.csv
    done
done

# add-c ... no yield, compare-and-swap synchronization
for i in "${threads[@]}"; do
    for j in "${iterations[@]}"; do
        ./lab2_add --threads=$i --iterations=$j --sync=c >> lab2_add.csv
    done
done

# add-yield-none ... yield, no synchronization 
for i in "${threads[@]}"; do
    for j in "${iterations[@]}"; do
        ./lab2_add --threads=$i --iterations=$j --yield >> lab2_add.csv
    done
done

# add-yield-m ... yield, mutex synchronization 
for i in "${threads[@]}"; do
    for j in "${iterations[@]}"; do
        ./lab2_add --threads=$i --iterations=$j --yield --sync=m >> lab2_add.csv
    done
done

# add-yield-s ... yield, spin-lock synchronization
for i in "${threads[@]}"; do
    for j in "${iterations[@]}"; do
        ./lab2_add --threads=$i --iterations=$j --yield --sync=s >> lab2_add.csv
    done
done

# add-yield-c ... yield, compare-and-swap synchronization
for i in "${threads[@]}"; do
    for j in "${iterations[@]}"; do
        ./lab2_add --threads=$i --iterations=$j --yield --sync=c >> lab2_add.csv
    done
done



### LIST TESTING ###

for j in 10, 100, 1000, 10000, 20000
do
    ./lab2_list --iterations=$j --threads=1 >> lab2_list.csv
done


for i in 1, 2, 4, 8, 12, 16, 24
do
    ./lab2_list --iterations=1000 --threads=$i >> lab2_list.csv
done

for i in 1, 2, 4, 8, 12, 16, 24
do
        ./lab2_list --iterations=1000 --threads=$i --sync=m >> lab2_list.csv
done

for i in 1, 2, 4, 8, 12, 16, 24
do
        ./lab2_list --iterations=1000 --threads=$i --sync=s >> lab2_list.csv
done

for i in "${list_threads[@]}"; do
    for j in "${list_iterations[@]}"; do
        ./lab2_list --threads=$i --iterations=$j >> lab2_list.csv
    done
done

for i in "${list_threads[@]}"; do
    for j in "${list_iterations[@]}"; do
        ./lab2_list --threads=$i --iterations=$j --yield=i >> lab2_list.csv
    done
done

for i in "${list_threads[@]}"; do
    for j in "${list_iterations[@]}"; do
        ./lab2_list --threads=$i --iterations=$j --yield=d >> lab2_list.csv
    done
done

for i in "${list_threads[@]}"; do
    for j in "${list_iterations[@]}"; do
        ./lab2_list --threads=$i --iterations=$j --yield=il >> lab2_list.csv
    done
done

for i in "${list_threads[@]}"; do
    for j in "${list_iterations[@]}"; do
        ./lab2_list --threads=$i --iterations=$j --yield=dl >> lab2_list.csv
    done
done

for i in "${list_threads[@]}"; do
    for j in "${list_iterations[@]}"; do
        ./lab2_list --threads=$i --iterations=$j --yield=i --sync=m >> lab2_list.csv
    done
done

for i in "${list_threads[@]}"; do
    for j in "${list_iterations[@]}"; do
        ./lab2_list --threads=$i --iterations=$j --yield=d --sync=m >> lab2_list.csv
    done
done

for i in "${list_threads[@]}"; do
    for j in "${list_iterations[@]}"; do
        ./lab2_list --threads=$i --iterations=$j --yield=il --sync=m >> lab2_list.csv
    done
done

for i in "${list_threads[@]}"; do
    for j in "${list_iterations[@]}"; do
        ./lab2_list --threads=$i --iterations=$j --yield=dl --sync=m >> lab2_list.csv
    done
done

for i in "${list_threads[@]}"; do
    for j in "${list_iterations[@]}"; do
        ./lab2_list --threads=$i --iterations=$j --yield=i --sync=s >> lab2_list.csv
    done
done

for i in "${list_threads[@]}"; do
    for j in "${list_iterations[@]}"; do
        ./lab2_list --threads=$i --iterations=$j --yield=d --sync=s >> lab2_list.csv
    done
done

for i in "${list_threads[@]}"; do
    for j in "${list_iterations[@]}"; do
        ./lab2_list --threads=$i --iterations=$j --yield=il --sync=s >> lab2_list.csv
    done
done

for i in "${list_threads[@]}"; do
    for j in "${list_iterations[@]}"; do
        ./lab2_list --threads=$i --iterations=$j --yield=dl --sync=s >> lab2_list.csv
    done
done















