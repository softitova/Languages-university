#ifndef HTTP_H
#define HTTP_H
#include <iostream>
#include <sys/socket.h>

enum STATE {BAD, FULL, PROGRESS, DEF};

struct http_request
{
    http_request(std::string);
    http_request(http_request &&);
    ~http_request();
    
    
    std::string get_host();
    std::string get_url();
    
    STATE get_state();
    void set_state(STATE);
    static bool is_request_ended(std::string const&);


    void cancel();
    bool is_canceled();
    void set_server(sockaddr);
    sockaddr get_server();
    void set_client(int);
    int get_client();
  

private:
    void parse_header();
    
    std::string host, url, header;
    sockaddr server;
    int client;
    std::atomic_bool canceled;
    STATE state = DEF;

};

struct http_response
{
    http_response();
    http_response(http_response const&);
    ~http_response();
    
    http_response& operator=(http_response const&);

    bool is_headert_ended();
    STATE get_state();
    void delete_request();
    void set_text(std::string);
    std::string get_text();
    void append_text(std::string);
    
    void set_url(std::string);
    std::string get_url();
    
    
    
    
private:
    STATE state = DEF;
    static const int text_limit = 1024;
    std::string text, req_url;
    
    
};

#endif
