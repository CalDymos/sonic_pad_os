#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include "wifi_event.h"
#include "wifi.h"
#include "wmg_debug.h"

extern void cancel_saved_conf_handle(const char *net_id);

static int get_net_ip(const char *if_name, char *ip, int *len, int *vflag)
{
    struct ifaddrs * ifAddrStruct=NULL, *pifaddr=NULL;
    void * tmpAddrPtr=NULL;

    *vflag = 0;
    getifaddrs(&ifAddrStruct);
    pifaddr = ifAddrStruct;
    while (pifaddr!=NULL) {
		if (NULL == pifaddr->ifa_addr) {
			pifaddr=pifaddr->ifa_next;
			continue;
        }

        printf("pifaddr->ifa_name = %s\n", pifaddr->ifa_name);
        printf("pifaddr->ifa_flags = %d\n", pifaddr->ifa_flags);
        printf("pifaddr->ifa_addr->sa_family = %d\n", pifaddr->ifa_addr->sa_family);

        if (pifaddr->ifa_addr->sa_family==AF_INET)
        {
            tmpAddrPtr=&((struct sockaddr_in *)pifaddr->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, tmpAddrPtr, ip, INET_ADDRSTRLEN);
        }
        else if (pifaddr->ifa_addr->sa_family==AF_INET6)
        {
            tmpAddrPtr=&((struct sockaddr_in6 *)pifaddr->ifa_addr)->sin6_addr;
            inet_ntop(AF_INET6, tmpAddrPtr, ip, INET6_ADDRSTRLEN);
        }
        else
        {
            tmpAddrPtr=&((struct sockaddr_in *)pifaddr->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, tmpAddrPtr, ip, INET_ADDRSTRLEN);
        }
        printf("pifaddr->ifa_addr->sin_addr = %s\n", ip);
        
        if (pifaddr->ifa_addr->sa_family==AF_INET) { // check it is IP4
            tmpAddrPtr=&((struct sockaddr_in *)pifaddr->ifa_addr)->sin_addr;
            if(strcmp(pifaddr->ifa_name, if_name) == 0){
                inet_ntop(AF_INET, tmpAddrPtr, ip, INET_ADDRSTRLEN);
                *vflag = 4;
                break;
            }
        } else if (pifaddr->ifa_addr->sa_family==AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in *)pifaddr->ifa_addr)->sin_addr;
            if(strcmp(pifaddr->ifa_name, if_name) == 0){
                inet_ntop(AF_INET6, tmpAddrPtr, ip, INET6_ADDRSTRLEN);
                *vflag=6;
                break;
            }
        }
        pifaddr=pifaddr->ifa_next;
    }

    if(ifAddrStruct != NULL){
        freeifaddrs(ifAddrStruct);
    }
    return 0;
}

int is_ip_exist()
{
    int len = 0, vflag = 0;
    char ipaddr[INET6_ADDRSTRLEN];

    get_net_ip("wlan0", ipaddr, &len, &vflag);
    return vflag;
}

void  start_udhcpc()
{
    int len = 0, vflag = 0, times = 0;
    char ipaddr[INET6_ADDRSTRLEN];
    char cmd[256] = {0}, reply[8] = {0};

    //modify by 101649,20210505
    //֮ǰִ��һ��udhcpcȥ����ip����ʱʱ��Ϊ30��
    //���ڸĳ�ִ��2��udhcpcȥ����ip����ʱʱ��Ϊ15��
    //try 2 times
    int totaltimes = 0;

    for(totaltimes = 0; totaltimes < 2; totaltimes++)
    {
        /* restart udhcpc */
        system("/etc/wifi/udhcpc_wlan0 restart");

    /* check ip exist */
    len = INET6_ADDRSTRLEN;
	w->StaEvt.event = WSE_ACTIVE_OBTAINED_IP;
	w->StaEvt.state = OBTAINING_IP;
	state_event_change(a->label);
	ms_sleep(1000);
    times = 0;
    do{
		ms_sleep(1000);
        get_net_ip("wlan0", ipaddr, &len, &vflag);
        times++;
    }while((vflag == 0) && (times < 15));

        if(vflag != 0)
            break;
        else
            wmg_printf(MSG_ERROR,"udhcpc wlan0 try %d times\n" , totaltimes);
    }
    wmg_printf(MSG_DEBUG,"vflag= %d\n",vflag);
    if(vflag != 0){
        wmg_printf(MSG_ERROR,"udhcpc wlan0 get ip success, %d times ipaddr:%s\n" , totaltimes , ipaddr);
		w->StaEvt.state = NETWORK_CONNECTED;
		state_event_change(a->label);
    }else{
        wmg_printf(MSG_ERROR,"udhcpc wlan0 timeout\n");
        if(w->StaEvt.state != CONNECTED){

			w->StaEvt.state = DISCONNECTED;
			w->StaEvt.event = WSE_OBTAINED_IP_TIMEOUT;
			cancel_saved_conf_handle(a->netIdConnecting);

			state_event_change(a->label);
        }
    }
}
