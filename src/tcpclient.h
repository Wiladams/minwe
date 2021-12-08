#pragma once

#include "network.h"


class TcpClient {
private:
    ASocket fSocket;
    IPHost  * fHost=nullptr;
    IPAddress fAddress;
    bool    fIsValid=false;
    int fLastError=0;

public:
    TcpClient(IPAddress &address)
        :fSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, false)
        , fAddress(address)
        , fLastError(0)
    {
        if (!fSocket.isValid()) {
            fLastError = fSocket.getLastError();
            return ;
        }

        // Try to connect, if failure
        // return immediately
        if (!connect()) {
            return;
        }
        
        fIsValid = true;
    }

    TcpClient(const char *hostname, const char * portname, bool hostNumeric=false)
        : fSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, false)
        , fIsValid(false)
    {
        // check if the socket was created
        if (!fSocket.isValid()) {
            fLastError = fSocket.getLastError();
            return ;
        }

        // Get address to host
        IPHost aHost(hostname, portname, AF_INET, SOCK_STREAM, hostNumeric);
        if (!fHost->isValid())
        {
            printf("could not find host: %s\n", hostname);
            return ;
        }

        fAddress = fHost->getAddress();
        
        // Try to connect, if failure
        // return immediately
        if (!connect()) {
            return;
        }

        fIsValid = true;
    }

    bool isValid() const {return fIsValid;}
    int getLastError() const {return fLastError;}

    bool close()
    {
        if (!fSocket.shutdown(SD_BOTH)) {
            fLastError = fSocket.getLastError();
            return false;
        } 
        return true;
    }

    bool connect()
    {
        int retCode = ::connect(fSocket.fSocket, &fAddress.fAddress, fAddress.fAddressLength);

        if (retCode != 0) {
            fLastError = WSAGetLastError();

            return false;
        }

        return true;
    }

    // This client should send the whole chunk
    std::tuple<int,int> send(const char *buff, const int buffLen, int flags = 0)
    {
        auto [error, result] = fSocket.send(buff, buffLen, flags);
        if (error != 0) {
            fLastError = WSAGetLastError();
            return { error, result };
        }

        return { 0, result };
    }

    std::tuple<int,int> receive(char *buff, int buffLen, int flags=0)
    {
        return fSocket.recv(buff, buffLen, flags);
    }
};

