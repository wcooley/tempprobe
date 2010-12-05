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

rrdtool create wirebird.rrd --step 2 \
	DS:probe0:GAUGE:300:U:U \
	DS:probe1:GAUGE:300:U:U \
	DS:probe2:GAUGE:300:U:U \
	RRA:AVERAGE:0.5:1:10000 \
	RRA:AVERAGE:0.5:5:10000 \
	RRA:AVERAGE:0.5:15:10000 \
	RRA:MIN:0.5:15:10000 \
	RRA:MAX:0.5:15:10000
