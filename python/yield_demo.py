#!/bin/env python
# -*- coding: utf-8 -*-
#########################################################################
# File Name: yield_demo.py
# Author: billowqiu
# mail: billowqiu@163.com
# Created Time: 2016-11-15 13:56:58
# Last Changed: 2016-11-15 13:58:43
#########################################################################
import time 

def fib():
    a, b = 0, 1
    while 1:
        yield b
        a, b = b, a+b

def caller():
    for num in fib():
        print num
        print '\n'
        time.sleep(1)

caller()

