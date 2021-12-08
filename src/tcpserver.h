#pragma once

#include "network.h"

class TcpServer {
private:
    ASocket fSocket;   // socket used to listen
    
    IPV4Address fServerAddress;
    bool fIsValid;
    int fLastError;

public:
    TcpServer(int porto, const char * interface = "localhost")
        :fSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, false),
        fServerAddress(porto)
    {
        if (!fSocket.isValid()) {
            // didn't get a valid socket, so we're
            // not valid
            fLastError = WSAGetLastError();

            return ;
        }

        if (!bind()) {
            return;
        }

        // listen
        if (!makePassive())
        {
            return ;
        }

        fIsValid = true;
    }

    virtual ~TcpServer() 
    {

    }

    bool isValid() const {return fIsValid;}
    int getLastError() const {return fLastError;}

    ASocket accept()
    {
        ASocket s = fSocket.accept();
        printf("TcpServer.accept(): %Id %d\n", s.fSocket, s.getLastError());
        if (!s.isValid()) {
            fLastError = fSocket.getLastError();
        }

        return s;
    }

    bool bindToPort(const short aPort, const int family=AF_INET) 
    {
        IPV4Address addr(aPort);

        int result = fSocket.bindTo(addr);

        if (result != 0) {
            fLastError = WSAGetLastError();
            return false;
        }

        return true;
    }

    bool bind()
    {
        int result = fSocket.bindTo(fServerAddress);

        if (result != 0) {
            fLastError = WSAGetLastError();
            return false;
        }

        fLastError = 0;
        return true;
    }

    // Typically when you're creating a server, you will 
    // need to tell the socket to start listening for
    // connections.  This is as opposed to be active, like
    // a client connection.
    bool makePassive(const int backlog=5)
    {
        bool success = fSocket.listen(backlog);
        if (!success) {
            fLastError = fSocket.getLastError();
        } else
        {
            fLastError = 0;
        }
        
        return success;
    }

    std::tuple<int,int> receive(char *buff, int buffLen)
    {
        return fSocket.recv(buff, buffLen, 0);
    }

    std::tuple<int, int> send(const char *buff, int buffLen)
    {
        return fSocket.send(buff, buffLen);
    }
};
