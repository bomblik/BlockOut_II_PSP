/*
   File:        Types.h 
  Description: Contains routines for accessing HTTP sites
  Program:     BlockOut
  Author:      Jean-Luc PONS

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
*/

#ifndef _HTTPINCLUDE_
#define _HTTPINCLUDE_

class Http {

  public:

    // Class constructor/desctructor
    Http();

    // Enable/disable proxy
    void SetProxy(BOOL enable);

    // Set proxy name
    void SetProxyName(char *name);

    // Set proxy port
    void SetProxyPort(int port);

    // Send a GET resquest and return response (timeout in millisec)
    // (return NULL when failed)
    char *Get(char *link,DWORD timeout,DWORD *outLength = NULL);

    // Get the last error message
    char *GetError();
        
    // Upload a file and return response (timeout in millisec)
    // Note: This function upload a file to a PHP script (PUT method)
    // (return NULL when failed)
    char *UploadFile(char *url,char *remotePHP,BYTE *buffer,DWORD length,DWORD timeout);

  private:

    BOOL CheckProxy();
    int  WaitFor(int sock,DWORD timeout,int mode);
    int  Write(int sock, char *buf, int bufsize,DWORD timeout);
    int  Read(int sock, char *buf, int bufsize,DWORD timeout);
    int  Connect(char *site, int port);
    int  GetHttpCode();

    char err_str[4096];
    char response[262144];
    char proxyName[256];
    int  proxyPort;
    BOOL useProxy;

};

#endif /* _HTTPINCLUDE_ */
