#!/bin/bash
gcc server.c -o server -O3 -s -Wall -Wextra
gcc client.c -o client -O3 -s -Wall -Wextra
