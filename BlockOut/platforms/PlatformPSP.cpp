/*
  File:        PlatformPSP.cpp 
  Description: Sony PSP platform code
  Program:     BlockOut
  Author:      bomblik

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
*/

#include "platforms/PlatformPSP.h"

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

int PlatformPSP::initWifi() {
  int err;

  if (!wifiInitialized) {

    sceUtilityLoadNetModule(1); /* common */
    sceUtilityLoadNetModule(3); /* inet */

    err = pspSdkInetInit();
    if (err != 0) {
        pspSdkInetTerm();
        return -1;
    }

    wifiInitialized = 1;
  }

  return 0;
}

int PlatformPSP::checkWifiButton() {
  if (sceWlanGetSwitchState() != 1) {
    return -1; /* turn wlan on */
  }

  return 0;
}

int PlatformPSP::connectToWifiNetwork() {
  int err;
  char szMyIPAddr[32];
  int iNetIndex = 0;

  for (iNetIndex = 1; iNetIndex < 100; iNetIndex++) /* skip the 0th connection */
  {
      char name[128];
      int state = 0;

      if (sceUtilityCheckNetParam(iNetIndex) != 0)
        continue;

      sceUtilityGetNetParam(iNetIndex, 0, (netData*) name);

      sceKernelDelayThread(1000*1000);

      err = sceNetApctlConnect(iNetIndex);
      if (err != 0) {
          /*connecting error*/
      }

      sceKernelDelayThread(1000*1000);

      err = sceNetApctlGetState(&state);

      if (err != 0)
      {
          /*connection failed*/
      }

      sceKernelDelayThread(8000*1000);

      /*get ip address*/
      if (sceNetApctlGetInfo(8, (char *) szMyIPAddr) != 0) {
          /*unknown ip address*/
      }
      else
      {
          /*ip address ok*/
          return 0;
      }
  }

  return -1;
}

