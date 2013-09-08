#include <stdio.h>
#define ONE_Gbps 1000000000
#define HUNDRED_Mbps 100000000
#define LINK_BANDWITDH HUNDRED_Mbps
#define RTT  0.050
#define BDP  (int)(LINK_BANDWITDH * RTT)/8  /* Bandwidth Delay Product */



main() {
    printf("%d\n", BDP);
    //  printf("%d\n", 2**3);
}
