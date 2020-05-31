#include <pcap.h>
#include <stdlib.h>
#include "workthread.h"

#if(AD_SAMPLE_ACCURACY==16)
static char packet_filter[] = "ip and udp and ether[29] & 0xff = 0x0a";
#else
//static char packet_filter[] = "ether[12] =0x88 and ether[13] = 0xbb";
static char packet_filter[] = "ether[12] =0x01 and ether[13] = 0xc0";
#endif

void *PcapThreadFunc(void *arg)
{
  (void)(arg);
  int i=0;
  char errBuf[PCAP_ERRBUF_SIZE];
  pcap_if_t *alldevs,*d;
  /* get a device */
  //devStr = pcap_lookupdev(errBuf);
  pcap_findalldevs(&alldevs,errBuf);

for(d=alldevs;d;d=d->next)
{
    printf("%d. %s\n",++i,d->name);
    //if(d->description)
    //printf(" (%s)\n",d->description);

}
//for(d=alldevs,i=0;i<inum-1;d=d->next,i++);

  /* open a device, wait until a packet arrives */
//pcap_t * pcap_open_live(const char * device, int snaplen, int promisc, int to_ms, char * errbuf)
//1表示混杂模式
//0表示一直等到数据包来

  pcap_t * device = pcap_open_live("eth0", 65535, 1, 0, errBuf);

  if(!device)
  {
    printf("error: pcap_open_live(): %s\n", errBuf);
    exit(1);
  }

  /* construct a filter */
  struct bpf_program filter;

  pcap_compile(device, &filter, packet_filter, 1, 0);
  pcap_setfilter(device, &filter);

  int id = 0;
  //-1表示永远抓包
  pcap_loop(device, -1, ethernet_protocol_packet_callback, (u_char*)&id);

  pcap_close(device);

  return nullptr;
}

