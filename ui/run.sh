#!/bin/bash
TDFW=/home/olimex/git/tdfw
echo olimex | sudo -S "$TDFW"/scoring/setup.sh
"$TDFW"/scoring/scoring | "$TDFW"/ui/finaltest.py
