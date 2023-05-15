#!/bin/sh

gdb $1 $2 --ex "bt" --ex "thread apply all bt" --ex "quit"
exit 0
