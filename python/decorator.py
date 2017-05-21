#!/bin/env python
# -*- coding: utf-8 -*-
#########################################################################
# File Name: decorator.py
# Author: billowqiu
# mail: billowqiu@163.com
# Created Time: 2016-06-04 16:14:51
# Last Changed: 2016-06-04 16:17:13
#########################################################################
def log(func):
    def wrapper(*argc, **kw):
        print('call %s()' % func.__name__)
        return func(*argc, **kw)
    return wrapper

@log
def now():
    print "20160604"

