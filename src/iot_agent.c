/*
 * Author: Matthias Hahn <matthias.hahnl@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/* 
 * sample: UDP communication with iotkit-agent. 
 * For TCP you would have to slightly modify socket parameters and bind to the socket 
 * rather than connect. Also the transmitted data is would have to contain the length of each message.
 * Pls consult documentation for further information.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/* node (host) and service (port) iotkit-agent is listening for UDP data
 * as defined in /etc/iotkit-agent/config.json */
#define NODE "localhost" 
#define SERVICE "41234" 


/**
 * @brief sends data value argv[2] on component argv[1] to cloud via iotkit-agent (e.g. "argv[0] temp 30").
 * Note: you have to register components prior. 
 * E.g. using "iotkit-admin register <comp_name> <catalogid>" or via iotkit-agent 
 */
int iot_agent_send(char *comp_name, char *value)
{
  int sfd = -1; // socket file descriptor - will be non-negative if successfully opened

  /* get addrinfo structur required to connect to socket */
  struct addrinfo hints;
  hints.ai_flags = 0;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = 0;
  hints.ai_addrlen = 0;
  hints.ai_addr = NULL;
  hints.ai_canonname = NULL;
  hints.ai_next = NULL;
  struct addrinfo *res;
  int err_code;
  err_code = getaddrinfo(NODE, SERVICE, &hints, &res);
  if (err_code != 0) {
    fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(err_code));
    return(EXIT_FAILURE);
  }

  struct addrinfo *p_ai = res;
  /* loop through the result list of addrinfo and 
   * try connecting each element until connection succeeds or all elements are tested */
  char success = 0;
  while (p_ai != NULL) {
    sfd = socket(p_ai->ai_family, p_ai->ai_socktype, IPPROTO_UDP);
    if (sfd == -1) { //try next if any left
      p_ai = p_ai->ai_next;
      continue; 
    }
    if (connect(sfd, p_ai->ai_addr, p_ai->ai_addrlen) == 0) {
      success = 1;
      break; // success
    }
    p_ai = p_ai->ai_next;
  } 
  freeaddrinfo(res);
  if (! success) {
    fprintf(stderr, "couldn't create socket\n");
    return(EXIT_FAILURE);
  }
 
  int len;
  len = strlen(comp_name) + strlen(value) + 15 ;
  char json_string[len];
  strcpy(json_string, "{\"n\":\"");
  strcat(json_string, comp_name);
  strcat(json_string, "\",\"v\":\"");
  strcat(json_string, value);
  strcat(json_string, "\"}");
  if (write(sfd, json_string, len) != len) {
    fprintf(stderr, "partial/failed write\n");
    return(EXIT_FAILURE);
  }
  return(EXIT_SUCCESS);
}
  
