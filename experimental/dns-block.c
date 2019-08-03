#define _GNU_SOURCE

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#include <dlfcn.h>
//#include <stdlib.h>
//#include <stdarg.h>

#include <fnmatch.h>

#define WHITELIST_LENGTH   20
#define BLACKLIST_LENGTH   2

const char *hostname_whitelist[WHITELIST_LENGTH] =
    { "ap.spotify.com", // audio (access point)
    "*.ap.spotify.com", // resolved access points
    "apresolve.spotify.com", // access point resolving
    "audio-sp-*.pscdn.co", // audio
    "audio4-fa.scdn.co", // audio
    "charts-images.scdn.co", // charts images
    "dailymix-images.scdn.co", // daily mix images
    "dealer.spotify.com", // discord rich presence
    "i.scdn.co", // cover art
    "lineup-images.scdn.co", // playlists lineup images
    "merch-img.scdn.co", // merch images
    "mosaic.scdn.co", // playlist mosaic images
    "o.scdn.co", // cover art
    "pl.scdn.co", // playlist images
    "profile-images.scdn.co", // artist profile images
    "seeded-session-images.scdn.co", // radio images
    "spclient.wg.spotify.com", // ads/tracking (blocked in blacklist), radio, recently played, friend activity,...
    "t.scdn.co", // background images
    "thisis-images.scdn.co", // 'this is' playlists images
    "video-fa.scdn.co", // videos
    };
    
const char *hostname_blacklist[BLACKLIST_LENGTH] =
    { "https://spclient.wg.spotify.com/ads/*",
    "https://spclient.wg.spotify.com/ad-logic/*", };
 
int getaddrinfo(const char *hostname, const char *service,
                const struct addrinfo *hints, struct addrinfo **res)
{
    printf("getaddrinfo request: '%s' '%s' ", hostname, service);
    
    int block = 1;
    
    int i;
    for (i = 0; i < WHITELIST_LENGTH; i++) {
        if (fnmatch(hostname_whitelist[i], hostname, FNM_NOESCAPE) == 0) {
            block = 0;
            break;
        }
    }
    
    for (i = 0; i < BLACKLIST_LENGTH; i++) {
        if (fnmatch(hostname_blacklist[i], hostname, FNM_NOESCAPE) == 0) {
            block = 1;
            break;
        }
    }
    
    if (block) {
        printf("BLOCKED\n");
        return -1;
    } else {
        printf("ALLOWED\n");
    }
    
    static typeof(getaddrinfo) *old_getaddrinfo = NULL;
    
    if (!old_getaddrinfo) {
        old_getaddrinfo = dlsym(RTLD_NEXT, "getaddrinfo");
    }
    
    if (!old_getaddrinfo) {
        printf("can't find regular getaddrinfo function\n");
        return -1;
    }
    
    return old_getaddrinfo(hostname, service, hints, res);
}
