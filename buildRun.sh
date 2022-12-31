#!/usr/bin/bash

./built/gclang.exe ./built/b.gc -o ./bGCOut -A -D
fasm ./bGCOut.S -m512000
./bGCOut
echo "exit code $?"