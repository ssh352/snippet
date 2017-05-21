# -*- coding: utf-8 -*-
from flask import Flask, render_template, request, redirect, make_response, render_template_string
from model import user
from model import testsql
import flask
from random import randint

app = Flask(__name__)
print app
print user.app


@app.route('/')
def hello_world():
    return render_template('index.html')


@app.route('/sse')
def sse():
    return render_template('sse.html')


@app.route('/sse_info', methods=['POST', 'GET'])
def sse_info():
    return 'sse-info'


@app.route('/init_testsql')
def init_testsql():
    print flask.current_app
    for i in range(100000):
        mobile = '13'
        for j in range(9):
            mobile += str(randint(0,9))
            testobj = testsql.TestSQL(i+10000, mobile, '2017-01-01 00:00:00', '2017-02-01 23:23:23')
        testsql.db.session.add(testobj)
    testsql.db.session.commit()
    testsqls = testsql.TestSQL.query.all()
    return str(testsqls)


@app.route('/user')
def get_user():
    print flask.current_app
    users = user.User.query.all()
    print repr(users[0])
    return users[1].username


@app.route('/user/<uname>/friend/<fname>')
def v_user_friend(uname, fname):
    return uname + '\'s friend - ' + fname + '\'s profile'


@app.route('/cookies')
def cookies_demo():
    #is_first = request.cookies['is_first']

    if not ('is_first' in request.cookies):
        response = make_response(redirect('/user'))
        response.set_cookie(key='is_first', value='1')
        return response
    else:
        return 'you are visit this site again'


@app.route('/genius')
def genius():
    return 'nothing special'


with app.test_request_context('/genius',method='GET'):
    print app.dispatch_request()


@app.route('/v_index')
def v_index():
    tpl = '''
        <div>your user agent is <b>{{ request.headers['User-Agent']}}</b></div>
        <div>you cookie is <b> {{ request.cookies }} </div>
        <div>format_price <b> {{ format_price(80) }} </div>
    '''
    return render_template_string(tpl)


@app.context_processor
def utility_processor():
    def format_price(amout, currency=u'$'):
        return u'{0:.2f}{1}'.format(amout, currency)
    return dict(format_price = format_price)


@app.route('/escape')
def escape():
    user = {'id':123,'nickname':'< IAMKING>'}
    tpl = '''
          <h1>homepage of <a href="/user/{{id}}">{{nickname|e}}</a></h1>
          '''
    return render_template_string(tpl, **user)

data = [
    {'name' : 'John', 'age' : 20, 'unit' : 'CIA'},
    {'name' : 'Linda', 'age' : 21,'unit' : 'FBI'},
    {'name' : 'Mary', 'age' : 30, 'unit' : 'FBI'},
    {'name' : 'Cook', 'age' : 40, 'unit' : 'CIA'}]


@app.route('/forloop')
def for_loop():
    tpl = u'''
      <ul>
          {% for user in users if user.unit == 'CIA' %}
          <li>{{ user.name }}</li>
          {% else %}
          <li>一个人都没有!</li>
          {% endfor %}
      </ul>
    '''
    return render_template_string(tpl,users=data)

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=8080)
