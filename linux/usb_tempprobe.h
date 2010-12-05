/*
 * usb_tempprobe.h
 * Developed for Prof. Bart Massey's CS572 at Portland State University
 * 	Fall 2010
 *
 * Copyright (c) 2010 by Freddy Carl, Wil Cooley and Akbar Saidov.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _USB_TEMPPROBE_H
#define _USB_TEMPPROBE_H

#define TEMPPROBE_MAGIC 0xFF
#define ENUM_CMD 	_IO(TEMPPROBE_MAGIC, 0)
#define RDALL_CMD 	_IO(TEMPPROBE_MAGIC, 1)

#define TEMPPROBE_IOCTL_MAX 1

#endif /* _USB_TEMPPROBE_H */
