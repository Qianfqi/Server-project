#include <iostream>
#include <map>
#include <functional>
#include <string>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#define PORT 8080
using namespace std;


using requesthandler = function<string(const string&)>;
map<string, requesthandler> get_table;
map<string, requesthandler> post_table;
void setuproutes()
{
    get_table["/"] = [](const string& request){
        return "hello world!";
    };
    get_table["/register"] = [](const string& request){
        return "Please use POST to register";
    };
    get_table["/login"] = [](const string& request){
        return "Please use POST to login";
    };
    post_table["/login"] = [](const string& request){
        return "login success!";
    };
    post_table["/register"] = [](const string& request){
        return "register success!";
    };
}

pair<string, string> parseHttpRequest(const string& request)
{
    size_t method_end = request.find(" ");
    string method = request.substr(0, method_end);
    size_t uri_end = request.find(" ", method_end + 1);
    string uri = request.substr(method_end + 1, uri_end - method_end - 1);
    return {method, uri};
}

string handlerequest(const string& method, const string& uri, const string& request)
{
    string response_body;
    if(method == "POST")
    {
        if(post_table.count(uri) == 0)  response_body = "404 NOT FOUND!!!";
        else    response_body = post_table[uri](request);
    }
    else if(method == "GET")
    {
        if(get_table.count(uri) == 0)  response_body = "404 NOT FOUND!!!";
        else    response_body = get_table[uri](request);
    }
    else   response_body = "Method Wrong";
    return response_body;
}
int main()
{
    int server_fd, new_socket;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    int addlen = sizeof(address);
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    
    bind(server_fd, (struct sockaddr*)&address, addlen);
    listen(server_fd, 3);
    setuproutes();
    while(true)
    {
        new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addlen);
        char buffer[1024] = {0};
        read(new_socket, buffer, 1024);
        string request(buffer);
        cout << request << endl;
        //string uri = request.substr(request.find(" ") + 1);
        // uri = uri.substr(0, uri.find(" "));
        auto [method, uri] = parseHttpRequest(request);
        string response_body;
        response_body = handlerequest(method, uri, request);
        cout << response_body << endl;
        string response = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\n" + response_body;
        send(new_socket, response.c_str(), response.size(), 0);
        close(new_socket);
    }
    return 0;
}