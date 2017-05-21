#!/bin/env python
# -*- coding: utf-8 -*-

from flask_sqlalchemy import SQLAlchemy

db = SQLAlchemy()

class Book(db.Model):
    __tablename__ = "Books"
 	
    BookId = db.Column(db.Integer, primary_key=True)
    Title = db.Column(db.String)      
    #AuthorId = Column(Integer, ForeignKey("Authors.AuthorId"))
    AuthorId = db.Column(db.Integer)

    def __str__():
    	return "%s:%s:%s" % (self.BookId, self.Title, self.AuthorId)                   