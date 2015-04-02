#!/bin/bash
TDFW=/home/olimex/git/tdfw
sudo "$TDFW"/scoring/setup.sh
"$TDFW"/scoring/scoring | "$TDFW"/ui/finaltest.py
