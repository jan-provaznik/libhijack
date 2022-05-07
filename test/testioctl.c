/* 2018 - 2022 Jan Provaznik (jan@provaznik.pro)
 *
 * Queries network interfaces for their hardware addresses 
 * using ioctl (2) with SIOCGIFHWADDR request (man 7 netdevice).
 */

#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/ip.h>

#define SIOCGIFHWADDR 0x8927

int probe (const char * name) {
  struct ifreq ifr;
  int offset;

  strcpy(ifr.ifr_name, name);

  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (0 == ioctl(sock, SIOCGIFHWADDR, & ifr)) {
    for (offset = 0; offset < 6; ++offset) {
      printf("%02x", ifr.ifr_hwaddr.sa_data[offset] & 0xff);
      if (offset < 5) {
        printf(":");
      }
    }
    printf("\n");
  }

  close(sock);
  return 0;
}

int main (int argc, char ** argv) {
  struct if_nameindex * ifhead;
  struct if_nameindex * ifiter;

  ifhead = if_nameindex();

  if (! ifhead) {
    perror("Failed to retrieve the list of network interfaces");
    return 1;
  }

  for (ifiter = ifhead; ifiter && ifiter->if_index; ++ifiter) {
    printf("%3i %-16s ", ifiter->if_index, ifiter->if_name);
    probe(ifiter->if_name);
  }

  if_freenameindex(ifhead);

  return 0;
}

