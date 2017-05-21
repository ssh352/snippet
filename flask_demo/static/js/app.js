/**
 * Created by qiutao on 2016/10/29.
 */
function loadXMLDoc()
{
    var xmlhttp;
    if (window.XMLHttpRequest)
    {
        // code for IE7+, Firefox, Chrome, Opera, Safari
        xmlhttp=new XMLHttpRequest();
    }
    xmlhttp.onreadystatechange=function()
    {
        if (xmlhttp.readyState==4 && xmlhttp.status==200)
        {
            document.getElementById("myDiv").innerHTML=xmlhttp.responseText;
        }
    }
    //xmlhttp.open("GET","http://192.168.20.222:808/home/check",true);
    xmlhttp.open("GET","http://localhost:8000/auth/user",true);
    //xmlhttp.setRequestHeader('abc', 'cde');
    xmlhttp.setRequestHeader("Authorization", "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJzdWIiOjUsImlzcyI6Imh0dHA6XC9cL2xvY2FsaG9zdDo4MDAwXC9hdXRoIiwiaWF0IjoxNDc4MjMyNDk2LCJleHAiOjE0NzgyMzYwOTYsIm5iZiI6MTQ3ODIzMjQ5NiwianRpIjoiYjM1ZmM4ZWIyMmU0Y2RmYzAxNGEwYTNiMDI4ODRlNWQifQ.xNJvzkfawGUG5uTGZ0plxxXylHZao28MnqSfoIm5zhk");


    xmlhttp.send();
}
