#!/bin/env python
# -*- coding: utf-8 -*-
#########################################################################
# File Name: flask_dashed_demo.py
# Author: billowqiu
# mail: billowqiu@163.com
# Created Time: 2016-10-22 22:30:57
# Last Changed: 2016-10-22 22:32:12
#########################################################################
from flask import Flask
from flask_dashed.admin import Admin

app = Flask(__name__)
admin = Admin(app)

if __name__ == '__main__':
    app.run(debug=True)
