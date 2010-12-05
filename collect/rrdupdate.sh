#!/bin/bash

RRD=wirebird.rrd

TEMP="$(sudo ../linux/readtemp b)"
TEMPFMTTED=$(echo -e "$TEMP" |awk -vFS='\t' '{printf "%.2f:%.2f:%.2f\n", $1/100, $2/100, $3/100}' )

rrdtool update "$RRD" "N:$TEMPFMTTED"
