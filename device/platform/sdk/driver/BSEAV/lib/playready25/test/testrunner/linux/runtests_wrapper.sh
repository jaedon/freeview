#!/bin/bash
# /*++
# Copyright (c) Microsoft Corporation.  All rights reserved.
# --*/

#
# This is a simple BASH script wrapper for running the PK tests on Linux.
# It first converts the line endings in the co-located 'runtests.sh' from Windows/DOS style to Unix style.
#
# This file must be edited/saved with Unix style line endings.
#
###################################################################################################

perl -pi -e 's/\r\n/\n/g' ./runtests.sh

. ./runtests.sh
