var http = require("http");
var url = require("url");
var mysql = require("mysql");
var application = {};
application.home = function (request, response) {
    var user_name = {};
    var followers = {};

    function return_function() {
        response.write("<!DOCTYPE html>\n<html>\n    <body>\n\n        <h4>\n            Followers of ");
        response.write("<a text = ");
        response.write('\"' + user_name.text + '\"');
        response.write("id = \"user_name\"");
        response.write(">Mike</a>:\n        </h4>\n        ");
        for (var followers_iterator = 0; followers_iterator < followers.data.lenght; followers_iterator++) {
            var item = followers.data[followers_iterator];
            var fimg = {};
            var fname = {};
            fname.inner_html = item.name;
            fimg.src = item.image;
            response.write("<li>\n                ");
            response.write("<img src = ");
            response.write('\"' + fimg.src + '\"');
            response.write("id = \"fimg\"");
            response.write("></img>\n                ");
            response.write("<a id=\"fname\">" + fname.inner_html + "</a>");
            response.write(">\n        </ul");
        }
        response.write(">\n\n    </body>\n</html>\n\n");
    }
    var user = {
        id: "admin"
    };
    user_name.text = user.id;
    var data = [];

    function __callback1() {
        function __callback2() {
            followers.data = data;
            return_function();
        }
        var follower_iterable = data;
        var follower_iterator = 0;

        function __for_code1() {
            var follower = follower_iterable[follower_iterator];
            function __callback3() {
                __for_iter1();
            }
            var __ws_options1_path = "/maps/api/directions/staticmap" + "?center=" + follower.address + "&zoom=" + 5 + "&sensor=" + false;
            var __ws_options1 = {
                host: 'http://maps.googleapis.com',
                port: 80,
                path: __ws_options1_path
            };
            http.get(__ws_options1, function (res) {
                var data = '';
                res.on('data', function (chunk) {
                    data += chunk;
                });

                function done() {
                    var return_value = data;
                    follower.map = return_value;
                    __callback3()
                }
                res.on('end', done);
                res.on('close', done);
            }).on('error', function (e) {
                console.log("Got error: " + e.message);
            });
        }

        function __for_iter1() {
            follower_iterator++;
            __for_cond1();
        }

        function __for_cond1() {
            if (follower_iterator < follower_iterable.length) __for_code1();
            else __callback2();
        }
        __for_cond1();
    }
    new mysql.Database({
        hostname: 'localhost',
        user: 'user',
        password: 'password',
        database: 'test'
    }).connect(function (error) {
        var __query1 = "SELECT * FROM FOLLOWERS\n               WHERE USERID = " + user.id + "";
        this.query(__query1).execute(function (error, rows, cols) {
            data = rows;
            __callback1();
        });
    });
};
http.createServer(function (request, response) {
    var pathname = url.parse(request.url).pathname;
    if (pathname == 'home') {
        application.home(request, response);
    }
}).listen(8888);