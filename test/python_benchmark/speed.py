#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2017 Peter Volgyesi <peter.volgyesi@gmail.com>
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

"""Based on Gabor's speed.py
"""
import time
import timeit
import dis

dis.dis("y = 2**0.5")

start = time.perf_counter()
y = 2**0.5
end = time.perf_counter()
elapsed = (end - start) * 1000.0
print("single run, elapsed time = {:.12f} milliseconds".format(elapsed))

N=100
elapsed = timeit.timeit("y = 2**0.5", number=N)
print("{} runs, average time = {:.12f} milliseconds".format(N, elapsed / N))
