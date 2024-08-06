#!/bin/bash

echo "Creating directory with 100 files of 1MB each"
time (
    mkdir -p directory1
    for i in {1..100}; do
        dd if=/dev/zero of=directory1/file${i}.txt bs=1M count=1
    done
)

echo "Creating directory with 10000 files of 10B each"
time (
    mkdir -p directory2
    for i in {1..10000}; do
        dd if=/dev/zero of=directory2/file${i}.txt bs=10 count=1
    done
)

echo "Creating directory with only 10B files, but only 10 directly, and sub-directory recursively..."
time (
    mkdir -p directory3
    for i in {1..10}; do
        dd if=/dev/zero of=directory3/file${i}.txt bs=10 count=1
    done
    for i in {1..10}; do
        mkdir -p directory3/subdir${i}
        for j in {1..10}; do
            dd if=/dev/zero of=directory3/subdir${i}/file${j}.txt bs=10 count=1
        done
    done
)

# Display the total number of files created
echo "Total number of files created:"
find . -type f | wc -l
