#include "http.h"

static const std::string dCRLF = "\r\n\r\n";
static const std::string CRLF = "\r\n";

http_request::http_request(std::string header_) : header(header_), canceled(false)
{
    parse_header();
}

http_request::http_request(http_request&& other) : host(std::move(other.host)), header(other.header),
                                         server(other.server), client(other.client), canceled(false) {}

http_request::~http_request() {}

std::string http_request::get_host()
{
    return host;
}

void http_request::cancel()
{
    canceled = true;
}

bool http_request::is_canceled()
{
    return canceled;
}

std::string http_request::get_url()
{
    return url;
}

STATE http_request::get_state()
{
    return state;
}

void http_request::set_state(STATE state_)
{
    state = state_;
}

void http_request::set_client(int client_)
{
    client = client_;
}

void http_request::set_server(sockaddr server_)
{
    server = server_;
}

int http_request::get_client()
{
    return client;
}

sockaddr http_request::get_server()
{
    return server;
}

void http_request::parse_header()
{
    {
        auto it = header.find("Host: ");
        if(it == header.npos)
        {
            state = BAD;
            return;
        }
        it += 6;
        host = header.substr(it, header.find(CRLF, it) - it);
    }
    
    {
        auto it = header.find(dCRLF) + dCRLF.size();
        std::string first_line = std::string{header.cbegin(), header.cbegin() + it};
        std::string body = header.substr(it);
        it = first_line.find(" ") + 1;
        header = std::string{first_line.cbegin(), first_line.cbegin() + it}
            + first_line.substr(first_line.find(host, it) + host.length())
            + body;
    }
    
    {
        auto it = header.find(" ");
        while (header[it] == ' ') it++;
        url =  host + header.substr(it, header.find(" ", it) - it);
    }
}

bool http_request::is_request_ended(std::string const& msg)
{
    size_t i = msg.find(dCRLF);
    if (i == std::string::npos)
    {
        return false;
    }
    if (std::string{msg.cbegin(), msg.cbegin() + 4} != "POST")
    {
        return true;
    }
    
    std::string chunk_str = "chunked";
    size_t j = msg.find(chunk_str);
    if(j != std::string::npos)
    {
        return (std::string(msg.end() - 7, msg.end()) == std::string(CRLF + "0" + dCRLF));
    }
        
    std::string msg_str = "Content-Length: ";
    j = msg.find(msg_str) + msg_str.length();
    size_t content_length = 0;
    while (msg[j] != '\r')
    {
        content_length *= 10;
        content_length += (msg[j++] - '0');
    }
    return msg.substr(i + 4).length() == content_length;
}


http_response::http_response() {}
http_response::http_response(http_response const& other) : state(other.state), text(other.text), req_url(other.req_url) {}
http_response::~http_response() {}


http_response& http_response::operator=(http_response const& other)
{
    text = other.text;
    req_url = other.req_url;
    state = other.state;
    return *this;
}

std::string http_response::get_text()
{
    return text;
}

std::string http_response::get_url()
{
    return req_url;
}

void http_response::set_text(std::string text_)
{
    text = text_;
}

void http_response::set_url(std::string url_)
{
    req_url = url_;
}

void http_response::append_text(std::string str)
{
    text.append(str);
}

void http_response::delete_request()
{
    req_url.clear();
}

STATE http_response::get_state()
{
    return state;
}

bool http_response::is_headert_ended()
{
    return text.find(dCRLF) != std::string::npos;
}
