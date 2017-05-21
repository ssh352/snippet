from flask import Flask
from flask_sqlalchemy import SQLAlchemy

app = Flask(__name__)
app.config['SQLALCHEMY_DATABASE_URI'] = 'mysql+pymysql://root:@localhost/classicmodels'
db = SQLAlchemy(app)

class TestSQL(db.Model):
    __tablename__ = 'TestSQL'
    id = db.Column('ID', db.Integer, primary_key=True)
    userid = db.Column('UserID', db.Integer)
    mobile = db.Column('Mobile', db.String(18))
    arrivedate = db.Column('ArriveDate', db.DateTime)
    adddate = db.Column('AddDate', db.DateTime)
    updatetime = db.Column('UpdateTime', db.TIMESTAMP)

    def __init__(self, userid, mobile, arrivedate, adddate):
        self.userid = userid
        self.mobile = mobile
        self.arrivedate = arrivedate
        self.adddate = adddate

    def __repr__(self):
        return 'TestObj: id: %d, userid: %d' % (self.id, self.userid)

