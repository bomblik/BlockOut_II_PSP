/*
   File:        Http.cpp 
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


#include "GLApp/GLApp.h"
#include "Http.h"
#include <stdio.h>
#include <errno.h>
#ifndef WINDOWS
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#define closesocket close
#else
#include <Winsock2.h>
#endif

#ifdef PLATFORM_PSP
#include "sys/unistd.h"
#include <pspkernel.h>
#include <pspwlan.h>
#include <pspsdk.h>
#include <pspctrl.h>
#include "psputility_netmodules.h"
#include "psputility_netparam.h"
#include "pspnet_apctl.h"
#include "pspnet_inet.h"
#include <pspnet_resolver.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>
#include "netdb.h"
#endif

#ifdef WINDOWS
// Initialize Winsock
void InitialiseWinsock() {
  WSADATA wsaData = {0};
  int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
    char message[256];
	sprintf(message,"WSAStartup failed (code=%d).\nNetwork features won't be available\n",iResult);
	MessageBox(NULL,message,"Warning",MB_OK|MB_ICONWARNING);
  }
}
#endif

#define WAIT_FOR_READ  1
#define WAIT_FOR_WRITE 2

// -------------------------------------------------------

Http::Http() {
  strcpy(err_str,"No error");
  useProxy = FALSE;
  strcpy(proxyName,"");
  proxyPort=0;
}

// -------------------------------------------------------

void Http::SetProxy(BOOL enable) {
  useProxy = enable;
}

// -------------------------------------------------------

void Http::SetProxyName(char *name) {
  strncpy(proxyName,name,255);
  proxyName[255]=0;
}

// -------------------------------------------------------

void Http::SetProxyPort(int port) {
  proxyPort = port;
}


// -------------------------------------------------------

int Http::WaitFor(int sock,DWORD timeout,int mode) {
  fd_set fdset;
  fd_set *rd = NULL, *wr = NULL;
  struct timeval tmout;
  int result;

  FD_ZERO (&fdset);
  FD_SET (sock, &fdset);
  if (mode == WAIT_FOR_READ)
    rd = &fdset;
  if (mode == WAIT_FOR_WRITE)
    wr = &fdset;

  tmout.tv_sec  = (long)(timeout / 1000);
  tmout.tv_usec = (long)(timeout % 1000) * 1000;

  do
    result = select (sock + 1, rd, wr, NULL, &tmout);
  while (result < 0 && errno == EINTR);

  if( result==0 ) {
    sprintf(err_str,"The operation timed out");
  } else if ( result < 0 ) {
    sprintf(err_str,"Tranmission error");
    return 0;
  }

  return result;
}

// -------------------------------------------------------

int Http::Write(int sock, char *buf, int bufsize,DWORD timeout) {

  int total_written = 0;
  int written = 0;

  while( bufsize > 0 )
  {
    // Wait
    if (!WaitFor(sock, timeout, WAIT_FOR_WRITE))
      return -1;

    // Write
    do
      written = send(sock, buf, bufsize, 0);
    while (written == -1 && errno == EINTR);

    if( written <= 0 )
       break;

    buf += written;
    total_written += written;
    bufsize -= written;
  }

  if( written < 0 ) {
    sprintf(err_str,"Tranmission error");
    return -1;
  }

  return total_written;

}

// -------------------------------------------------------

int Http::Read(int sock, char *buf, int bufsize,DWORD timeout) {

  int rd = 0;
  int total_read = 0;

  while( bufsize>0 ) {

    // Wait
    if (!WaitFor(sock, timeout, WAIT_FOR_READ))
      return -1;

    // Read
    do
      rd = recv(sock, buf, bufsize, 0);
    while (rd == -1 && errno == EINTR);

    if( rd <= 0 )
      break;

    buf += rd;
    total_read += rd;
    bufsize -= rd;
  
  }

  if( rd < 0 ) {
    sprintf(err_str,"Tranmission error");
    return -1;
  }

  return total_read;

}

// -------------------------------------------------------

int Http::Connect(char *site, int port) {
  struct hostent *host_info;
  struct sockaddr_in server;

  // Resolve IP
  host_info = gethostbyname(site);
  if (host_info == NULL) {  
     sprintf(err_str, "Unknown host: %s", site);
     return -1; 
  }

  // Build TCP connection
  int sock = socket(AF_INET, SOCK_STREAM,0);
  if (sock < 0 ) {
#ifdef WINDOWS
    int errCode = WSAGetLastError();
    switch(errCode) {
      case WSAEACCES:
        sprintf(err_str,"Socket error: Permission denied");
        break;
      case WSAEMFILE:
        sprintf(err_str,"Socket error: Descriptor table is full");
        break;
      case WSAEPROTONOSUPPORT:
        sprintf(err_str,"Socket error: Specified protocol is not supported");
        break;
      default:
        sprintf(err_str,"Socket error: code:%d",errCode);
        break;
    }
#else
    switch(errno) {
      case EACCES:
        sprintf(err_str,"Socket error: Permission denied");
        break;
      case EMFILE:
        sprintf(err_str,"Socket error: Descriptor table is full");
        break;
      case ENOMEM:
        sprintf(err_str,"Socket error: Insufficient user memory is available");
        break;
      case ENOSR:
        sprintf(err_str,"Socket error: Insufficient STREAMS resources available");
        break;
      case EPROTONOSUPPORT:
        sprintf(err_str,"Socket error: Specified protocol is not supported");
        break;
      default:
        sprintf(err_str,"Socket error: Unknow code:%d",errno);
        break;
    }
#endif
    return -1;
  }  

  // Connect
  memset(&server,0,sizeof(sockaddr_in));
  server.sin_family = host_info->h_addrtype;
  memcpy((char*)&server.sin_addr, host_info->h_addr,host_info->h_length);
  server.sin_port=htons(port);
  if( connect(sock,(struct sockaddr *)&server, sizeof(server) ) < 0 ) {
    sprintf(err_str, "Cannot connect to host: %s\n", site);
    closesocket(sock);
    return -1;
  }

  // Sucess
  return sock;
}

// -------------------------------------------------------

int Http::GetHttpCode() {

  char err[4];
  strncpy( err , &(response[9]) , 3);
  err[3]=0;
  return atoi(err);

}

// -------------------------------------------------------

BOOL Http::CheckProxy() {

  if( useProxy ) {

    if( strlen(proxyName)==0 ) {
      sprintf(err_str,"Invalid HTTP proxy name");
      return FALSE;
    }

    if( proxyPort<=0 || proxyPort>65535 ) {
      sprintf(err_str,"Invalid HTTP proxy port number");
      return FALSE;
    }

  }

  return TRUE;

}

// -------------------------------------------------------

char *Http::UploadFile(char *url,char *remotePHP,BYTE *buffer,DWORD length,DWORD timeout)
{

  char page[256];
  char req[1024];
  strcpy(err_str,"");
  char link[512];

  if( !CheckProxy() ) return FALSE;

  if( remotePHP[0]=='/' ) {
    strcpy(page,remotePHP);
  } else {
    sprintf(page,"/%s",remotePHP);
  }

  // Connect
  int sock;
  if( useProxy ) {
    // Connect to the proxy
    sock  = Connect(proxyName,proxyPort);
    sprintf(link,"http://%s%s",url,page);
  } else {
    // Default HTTP connection
    sock  = Connect(url,80);
  }
  if( sock < 0 )
    return NULL;

  // Send the request
  sprintf(req,"PUT %s HTTP/1.0\r\n"
              "User-Agent: BlockOut\r\n"
              "Host: %s\r\n"
              "Content-Length: %d\r\n"
               "Cache-Control: no-cache\r\n"
              "\r\n",
              (useProxy?link:page),url,length);

  int lgth = (int)strlen(req);
  int w = Write(sock,req,lgth,timeout);
  if( w != lgth ) {
    if( strlen(err_str)==0 ) 
      sprintf(err_str,"HTTP send error");
    closesocket(sock);
    return NULL;
  }

  // Write PUT Data
  w = Write(sock,(char *)buffer,length,timeout);
  if( w != length ) {
    if( strlen(err_str)==0 ) 
      sprintf(err_str,"HTTP send error");
    closesocket(sock);
    return NULL;
  }

  // Get the response
  int r = Read(sock,response,262143,timeout);
  if( r<=0 ) {
    if( strlen(err_str)==0 ) 
      sprintf(err_str,"HTTP receive error");
    closesocket(sock);
    return NULL;
  }

  closesocket(sock);
  response[r]=0;

  // Check HTTP response
  int httpCode = GetHttpCode();

  if( httpCode>=300 ) {
    sprintf(err_str,"HTTP error code %d",httpCode);
    return NULL;
  }

  // Get RecResp
  char *p = strstr(response,"RecResp:");
  if( p==NULL ) {
    sprintf(err_str,"Invalid HTTP response");
    return NULL;
  }
  char *rr = p + 8;
  while( *p>=32 ) p++;
  *p=0;

  return rr;

}

// -------------------------------------------------------

char *Http::Get(char *link,DWORD timeout,DWORD *outLength) {

  char site[256];
  char page[256];
  char req[1024];
  strcpy(err_str,"");
  
  if( outLength ) *outLength = 0;

  if( !CheckProxy() ) return FALSE;

  if( strncmp(link,"http://",7) != 0 ) {
    sprintf(err_str,"Invalid URL address");
    return NULL;
  }

  strcpy(site,link+7);
  char *p = strchr(site,'/');
  if( !p ) {
    sprintf(err_str,"Invalid URL address");
    return NULL;
  }

  strcpy(page,p);
  *p = 0;

  // Connect
  int sock;
  if( useProxy ) {
    // Connect to the proxy
    sock  = Connect(proxyName,proxyPort);
  } else {
    // Default HTTP connection
    sock  = Connect(site,80);
  }
  
  if( sock < 0 )
    return NULL;

  // Build the request
  sprintf(req,"GET %s HTTP/1.0\r\n"
              "User-Agent: BlockOut\r\n"
              "Host: %s\r\n"
               "Cache-Control: no-cache\r\n"
              "\r\n",
              (useProxy?link:page),site);

  // Send the request
  int lgth = (int)strlen(req);
  int w = Write(sock,req,lgth,timeout);
  if( w != lgth ) {
    if( strlen(err_str)==0 ) 
      sprintf(err_str,"HTTP send error");
    closesocket(sock);
    return NULL;
  }

  // Get the response
  int r = Read(sock,response,262143,timeout);
  if( r<=0 ) {
    if( strlen(err_str)==0 ) 
      sprintf(err_str,"HTTP receive error");
    closesocket(sock);
    return NULL;
  }

  closesocket(sock);
  response[r]=0;

  // Check HTTP response
  int httpCode = GetHttpCode();

  if( httpCode>=300 ) {
    sprintf(err_str,"HTTP error code %d",httpCode);
    return NULL;
  }

  // Jump headers
  p = strstr(response,"\r\n\r\n");
  if( p==NULL ) {
    sprintf(err_str,"Invalid HTTP response");
    return NULL;
  }
  
  if( outLength ) *outLength = r - (int)((p+4)-response);

  return p+4;

}

// -------------------------------------------------------

char *Http::GetError() {

  return err_str;

}
