#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include "httplib.h"

using namespace httplib;

void callback(const Request& req,Response& res)
{
    (void)req;
    res.set_content("hello we_gobang","text/plain");
}

int main()
{
    Server http_server;
    http_server.Get("/aaa",callback);
    http_server.Get("/abc",[](const Request& req,Response& res){
            res.set_content("It's test to lambda","text/plain");
            });
    http_server.set_mount_point("/","./www");
    http_server.listen("0.0.0.0",18989);

    return 0;
}

