#!/bin/env python
# -*- coding: utf-8 -*-
#########################################################################
# File Name: sqlalchemy_demo.py
# Author: billowqiu
# mail: billowqiu@163.com
# Created Time: 2017-02-28 17:47:09
# Last Changed: 2017-03-08 21:52:07
#########################################################################
from sqlalchemy import create_engine, Table, Column, Integer, String, MetaData, inspect
import sqlalchemy
from sqlalchemy.sql import select

print dir(sqlalchemy.sql)
'''
eng = create_engine("mysql+pymysql://root:@localhost/flask_demo", echo=True)
meta = MetaData()
meta.reflect(bind=eng)

for attr in dir(meta):
    print attr

for table_name in meta.tables:
    print table_name

inspect = inspect(eng)
print inspect.get_table_names()
print inspect.get_schema_names()

#for attr in dir(inspect):
#    print attr

with eng.connect() as conn:
    meta = MetaData(eng)
    cars = Table('Cars', meta, autoload=True)
    print dir(cars.c.Name)

    stm = select([cars])
    rs = conn.execute(stm)

    print rs.fetchall()




from sqlalchemy import create_engine, ForeignKey
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy import Column, Integer, String
from sqlalchemy.orm import sessionmaker, relationship

Base = declarative_base()

class Author(Base):
    __tablename__ = "Authors"
 
    AuthorId = Column(Integer, primary_key=True)
    Name = Column(String)  
    Books = relationship("Book")

class Book(Base):
    __tablename__ = "Books"
 
    BookId = Column(Integer, primary_key=True)
    Title = Column(String)      
    #AuthorId = Column(Integer, ForeignKey("Authors.AuthorId"))
    AuthorId = Column(Integer)    
                           
    Author = relationship("Author")

with eng.connect() as con:
    
    meta = MetaData(eng)
    
    authors = Table('Authors', meta, autoload=True)
    books = Table('Books', meta, autoload=True)

    stm = select([authors.join(books, authors.c.AuthorId == books.c.AuthorId)])
    rs = con.execute(stm) 

    for row in rs:
        print 'name: %s, title: %s' % (row['Name'], row['Title'])


from sqlalchemy import create_engine
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy import Column, Integer, String
from sqlalchemy.orm import sessionmaker

Base = declarative_base()
 
class Car(Base):
    __tablename__ = "Cars"
 
    Id = Column(Integer, primary_key=True)
    Name = Column(String)  
    Price = Column(Integer)
        
Base.metadata.bind = eng        
Base.metadata.create_all()        
        
Session = sessionmaker(bind=eng)
ses = Session()    

# ses.add_all(
#    [Car(Id=1, Name='Audi', Price=52642), 
#     Car(Id=2, Name='Mercedes', Price=57127),
#     Car(Id=3, Name='Skoda', Price=9000),
#     Car(Id=4, Name='Volvo', Price=29000),
#     Car(Id=5, Name='Bentley', Price=350000),
#     Car(Id=6, Name='Citroen', Price=21000),
#     Car(Id=7, Name='Hummer', Price=41400),
#     Car(Id=8, Name='Volkswagen', Price=21600)])
#ses.commit()

# rs = ses.query(Car).all()

# for car in rs:
#     print car.Name, car.Price


                           
         
Session = sessionmaker(bind=eng)
ses = Session()   

res = ses.query(Author).filter(Author.Name=="Leo Tolstoy").first()
print res.AuthorId
for book in res.Books:
     print book.Title

#res = ses.query(Book).filter(Book.Title=="Emma").first()    
#print res.Author.Name
'''

from sqlalchemy import create_engine
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy import Column, Integer, String, DateTime, Float, SmallInteger
from sqlalchemy.orm import sessionmaker
eng = create_engine('mysql+pymysql://root:@localhost/ekatong', echo=True)
Base = declarative_base()
'''
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `member_id` int(11) NOT NULL,
  `cardsn` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `enter_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `tour_id` int(11) NOT NULL,
  `device_id` varchar(32) COLLATE utf8_unicode_ci NOT NULL,
  `check_status` double(16,13) NOT NULL,
  `faceurl` varchar(512) COLLATE utf8_unicode_ci NOT NULL,
  `violate` tinyint(4) NOT NULL,
  `idcard` varchar(18) COLLATE utf8_unicode_ci DEFAULT NULL,
  `check_type` tinyint(4) NOT NULL DEFAULT '0',
  `pictype` int(11) NOT NULL,
  `error_code` int(11) NOT NULL,
  `script_check_status` double(16,13) NOT NULL,
  `idcardinputtype` int(11) NOT NULL DEFAULT '0',
  `created_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `updated_at` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `deleted_at` timestamp NULL DEFAULT NULL,
'''
class EnterLog(Base):
    __tablename__ = 'tour_enter_logs_201612'

    id = Column(Integer, primary_key=True)
    member_id = Column(Integer)
    enter_at = Column(DateTime)
    cardsn = Column(String)
    tour_id = Column(Integer)
    device_id = Column(Integer)
    check_status = Column(Float)
    faceurl = Column(String)
    violate = Column(SmallInteger)
    pictype = Column(Integer)
    error_code = Column(Integer)
    script_check_status = Column(Float)


Session = sessionmaker(bind=eng)
ses = Session()    

e1 = EnterLog(member_id=10010, cardsn='billowqiu', tour_id=100, device_id=101, 
    check_status=100.0, faceurl='', violate=2, pictype=0, error_code=0, script_check_status=108.0)
ses.add(e1)
ses.commit()

