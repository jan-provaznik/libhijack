/* 2018 - 2022 Jan Provaznik (jan@provaznik.pro)
 *
 * Intercepts ioctl(fd, req, arg) calls. 
 *
 * If req matches SIOCGIFHWADDR (man 7 netdevice), that is, request for 
 * the hardware address of any network interface, the ifr_hwaddr field of 
 * the ifreq structure (man 7 netdevice) is populated by the address 
 * given by HIJACK_IOCTL_TARGET.
 *
 * Does not matter what network interface the address was requested for.
 */

#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <net/if.h>

#define SIOCGIFHWADDR 0x8927
#define DEF_HIJACK_IOCTL_TARGET "HIJACK_IOCTL_TARGET"

static uint64_t
getenvhexull (
  const char * name
) {

  char * value = getenv(name);

  if (! name) {
    return 0;
  }

  return strtoll(value, 0, 16);
}

static int
(* ioctl_next) (
  int fd,
  int request,
  void * payload
) = 0;

int
ioctl (
  int fd,
  int request,
  void * payload
) {

  if (! ioctl_next) {
    ioctl_next = dlsym(RTLD_NEXT, "ioctl");
  }

  if (! ioctl_next) {
    return - 1;
  }

  if (request == SIOCGIFHWADDR) {

    struct ifreq ifr;
    uint64_t address, offset;

    address = getenvhexull(DEF_HIJACK_IOCTL_TARGET);

    memcpy(& ifr, payload, sizeof(struct ifreq));
    for (offset = 0; offset < 6; ++offset) {
      ifr.ifr_hwaddr.sa_data[offset] = (address >> (6 - (offset + 1)) * 8) & 0xff;
    }
    memcpy(payload, & ifr, sizeof(struct ifreq));

    return 0;
  }

  return ioctl_next(fd, request, payload);
}

// __attribute__((constructor))
// static void libinit () {
// }

/* cc -fPIC -shared -ldl hijackioctl -o libhijackioctl.so */

