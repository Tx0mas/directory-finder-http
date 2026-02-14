#include<iostream>
#include <cstring>
#include <vector>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <fstream>

#include <mutex> 
std::mutex mtx;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr<<"Debe dar su lista.";
        return 1;
    }

    std::ifstream listaPalabras{argv[1]};
    if (!listaPalabras)
    {
        std::cerr<<"Fallo al abrir la file";
        return 1;
    }

    std::string url{};
    std::cout<<"Inserte la url a buscar los sub-directorios: ";
    std::cin>>url;

    struct addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *res{}; 

    int getInfo = getaddrinfo(url.c_str(),"80",&hints,&res);
    if (getInfo<0)
    {
        std::cerr<<"Error al agarrar info."<<'\n';
        return 1;
    }


    std::string palabra{};
    std::vector<std::pair<std::string,std::string>> upOrDown{}; 
    while (std::getline(listaPalabras,palabra))
    {
        int ourSocket = socket(res->ai_family,res->ai_socktype,res->ai_protocol);

        int getConnection = connect(ourSocket,res->ai_addr,res->ai_addrlen);
        if (getConnection<0)
        {
            std::cerr<<"Error al conectar"<<'\n';
            return 1;
        }

        std::string request = 
        "GET /"+palabra+"/ HTTP/1.1\r\n"
        "Host: " +url+ "\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)\r\n" 
        "Connection: close\r\n"
        "\r\n";

        std::vector<char> buffer(1024);
        send(ourSocket,request.c_str(),request.size(),0);

        size_t bytesRecibidos{};

        bytesRecibidos = recv(ourSocket,buffer.data(),sizeof(buffer),0);
        if (bytesRecibidos<=0)
        {
            break;
        }
        std::string buffer_string{buffer.data(),bytesRecibidos};
        std::cout<<buffer_string<<'\n';
        int checked{};
        for (size_t i{};i<bytesRecibidos;i++)
        {
            if (buffer_string.find("404")!=std::string::npos) //esto luego mejorarlo.
            {
                std::pair<std::string,std::string> par= {palabra,"DOWN"};
                upOrDown.push_back(par);
                checked=1;
                break;
            }
            else if (buffer_string.find("301")!=std::string::npos) 
            {
                std::pair<std::string,std::string> par= {palabra,"MOVED"};
                upOrDown.push_back(par);
                checked=1;
                break;
            }
        }
        if (checked!=1)
        {
            std::pair<std::string,std::string> par= {palabra,"UP"};
            upOrDown.push_back(par);
        }
    }

    for (auto par : upOrDown)
    {
        std::cout<<par.first<<": "<<par.second<<'\n';
    }


    return 0;
}
