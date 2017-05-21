#!/bin/env python
# -*- coding: utf-8 -*-
import os
from flask import Flask
import flask
from flask import request, render_template
from flask_login import LoginManager, UserMixin, login_required, current_user, login_user
from flask_bootstrap import Bootstrap

import sys
reload(sys)
sys.setdefaultencoding('utf-8')

import forms
import models

class User(UserMixin):
	def __init__(self):
		self.email = None
		self.password = None
		self.id = None
	def get_id(self):
		return self.email

user1 = User()
user1.email = 'billowqiu@163.com'
user1.password = '123321'
user1.id = 20000

user_dict = {}
user_dict[user1.email] = user1

app = Flask(__name__)
app.config['SECRET_KEY'] = os.urandom(32)
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:////Users/qiutao/code/flask_demo/test.db'
app.config['SQLALCHEMY_ECHO'] = True
login_manager = LoginManager(app)
bootstrap = Bootstrap(app)

#延迟初始化
db = models.db
print "before init_app"
db.init_app(app)
print "after init_app"

@login_manager.user_loader
def load_user(user_id):
	print "load_user: %s" % user_id
	return user_dict.get(user_id)


@app.route('/init')
def init():
	db.drop_all()
	db.create_all()

	book = models.Book()
	book.BookId = 10000
	book.Title = "python"
	book.AuthorId = 20000
	db.session.add(book)

	book = models.Book()
	book.BookId = 10001
	book.Title = "html"
	book.AuthorId = 20000
	db.session.add(book)

	book = models.Book()
	book.BookId = 10002
	book.Title = "javascript"
	book.AuthorId = 20001
	db.session.add(book)

	db.session.commit()

	return "initdb ok"

@app.route('/login', methods=['GET', 'POST'])
def login():
	if request.method == 'GET':
		form = forms.LoginForm()
		return render_template('login.html', form=form)
	elif request.method == 'POST':
		email = request.form.get('email')
		password = request.form.get('password')
		print "email:pass: %s:%s" % (email,password)

		user = user_dict.get(email)

		if user is not None and (user.password == password):
			login_user(user)
			print "after login_user: %s " % current_user
			flask.flash('成功登录')
			print "登录成功"
	        return flask.redirect(flask.url_for('demo'))

		return email+':'+password

@app.route('/registe')
def registe():
	form = forms.RegisteForm()
	return render_template('registe.html', form=form)

@app.route('/books')
@login_required
def books():
	user = current_user
	print "current_user id: %s" % user.id
	books = models.Book.query.filter_by(AuthorId=user.id).all()
	print books[0].Title
	return str(len(books))

@app.route('/demo')
@login_required
def demo():
	print current_user
	form = forms.RegisteForm()
	return render_template('demo.html', form=form)

if __name__ == '__main__':
	app.run(debug=True)