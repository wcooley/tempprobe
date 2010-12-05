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

TEMP="$(sudo ../linux/readtemp b)"
TEMPFMTTED=$(echo -e "$TEMP" |awk -vFS='\t' '{printf "%.2f:%.2f:%.2f\n", $1/100, $2/100, $3/100}' )

rrdtool update "$RRD" "N:$TEMPFMTTED"
