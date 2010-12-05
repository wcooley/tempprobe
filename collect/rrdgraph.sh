#!/bin/bash

RRD=wirebird.rrd
GRAPH=wirebird.png

rrdtool graph $GRAPH.new -A -l 10 -w 1100 -h 700 -S 2 --start now-15m \
	-z -n DEFAULT:6: -v "Degrees C" -r \
	COMMENT:"Current time\\:\t$(date +%H\\:%M\\:%S)\l" \
	DEF:Probe0=${RRD}:probe0:AVERAGE \
	LINE2:Probe0#3333ff:"Thermistor0\t" \
	GPRINT:Probe0:LAST:"Last\:%8.2lf" \
	GPRINT:Probe0:MIN:"Min\:%8.2lf" \
	GPRINT:Probe0:MAX:"Max\:%8.2lf\l" \
	DEF:Probe1=${RRD}:probe1:AVERAGE \
	LINE2:Probe1#00cc00:"Thermistor1\t" \
	GPRINT:Probe1:LAST:"Last\:%8.2lf" \
	GPRINT:Probe1:MIN:"Min\:%8.2lf" \
	GPRINT:Probe1:MAX:"Max\:%8.2lf\l" \
	DEF:Probe2=${RRD}:probe2:AVERAGE \
	LINE2:Probe2#ff6600:"Null probe	" \
	GPRINT:Probe2:LAST:"Last\:%8.2lf" \
	GPRINT:Probe2:MIN:"Min\:%8.2lf" \
	GPRINT:Probe2:MAX:"Max\:%8.2lf\l" \

mv $GRAPH.new $GRAPH
