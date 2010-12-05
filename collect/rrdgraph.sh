#!/bin/bash
#
# Copyright (c) 2010 by Freddy Carl, Wil Cooley and Akbar Saidov.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


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
