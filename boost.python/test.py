#!/bin/env python
# -*- coding: utf-8 -*-
#########################################################################
# File Name: test.py
# Author: billowqiu
# mail: billowqiu@163.com
# Created Time: 2017-11-15 09:31:49
# Last Changed: 2017-11-17 10:25:51
#########################################################################
import extending

h = extending.hello("abc")
f = h.get_foo()

print "type(h.vf): %s" % type(h.vf)

print "type(f): %s" % type(f)

print "type(f.vi): %s" % type(f.vi)
print f.vi
print dir(f.vi)
l = []
print l.__class__
print dir(l)
