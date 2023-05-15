#!/bin/sh

gdb $1 $2 --ex "set pagination off" --ex "bt" --ex "thread apply all bt" --ex "detach" --ex "quit"
exit 0
