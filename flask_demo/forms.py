#!/bin/env python
# -*- coding: utf-8 -*-
from flask_wtf import FlaskForm
from wtforms import StringField, SubmitField, PasswordField
from wtforms.validators import Required, Email, Length

class LoginForm(FlaskForm):
	email = StringField("邮箱:", validators=[Required(), Email()])
	password = PasswordField("密码：", validators=[Required(), Length(3,6)])
	submit = SubmitField("提交")

class RegisteForm(FlaskForm):
	email = StringField("邮箱：", validators=[Required(), Email()])
	submit = SubmitField("提交")

class DemoForm(FlaskForm):
	pass