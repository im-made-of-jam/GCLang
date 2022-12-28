format elf64 executable 3

entry __main

__startup:

__shutdown:
segment readable executable
call __startup
