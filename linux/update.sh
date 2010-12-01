#!/bin/bash

RRD=wirebird.rrd
GRAPH=wirebird.png

rrdtool update $RRD N:$(./user b|awk -F'\t' '{printf "%.2f:%.2f:%.2f\n" $1/100 $2/100 $3/100}' 2> /dev/null)

rrdtool graph $GRAPH -w 320 -h 220 --start now-8hr --end now \
	--font DEFAULT:0:Inconsolata \
	-n DEFAULT:6: -v "Degrees F" -u 400 -l 30 -r \
	HRULE:160#ff0000:"Done!	" \
	HRULE:200#cc33cc:"Smoker Lower Range	" \
	HRULE:250#ff33cc:"Smoker Upper Range\n" \
	DEF:Ambient=${RRD}:probe0:AVERAGE \
	LINE2:Ambient#3333ff:"Ambient	" \
	GPRINT:Ambient:LAST:"Last\:%8.2lf" \
	GPRINT:Ambient:MIN:"Min\:%8.2lf" \
	GPRINT:Ambient:MAX:"Max\:%8.2lf\n" \
	DEF:Smoker=${RRD}:probe1:AVERAGE \
	LINE2:Smoker#00cc00:"Smoker	" \
	GPRINT:Smoker:LAST:"Last\:%8.2lf" \
	GPRINT:Smoker:MIN:"Min\:%8.2lf" \
	GPRINT:Smoker:MAX:"Max\:%8.2lf\n" \
	DEF:Turkey=${RRD}:probe2:AVERAGE \
	LINE2:Turkey#ff6600:"Bird	" \
	GPRINT:Turkey:LAST:"Last\:%8.2lf" \
	GPRINT:Turkey:MIN:"Min\:%8.2lf" \
	GPRINT:Turkey:MAX:"Max\:%8.2lf\n" \
