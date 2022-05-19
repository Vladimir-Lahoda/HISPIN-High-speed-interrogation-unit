// Tutorial about UDP socket in LINUX: https://www.binarytides.com/programming-udp-sockets-c-linux/
// Reference of WiringPi library: http://wiringpi.com/
#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdint.h>
#include <errno.h>

#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>	

#define PIN 25

#define BUFLEN 65536	//Max length of buffer
#define RPI_PORT 32768
#define PC_PORT 32780
#define ADDR_PC "192.168.1.1"

uint8_t buf[35000] = {};
uint8_t *ps;
struct sockaddr_in si_me, si_other;
	
int s, i, slen = sizeof(si_other) , recv_len;
uint8_t send_flag = 0;

void SPI_TXRX(void)
{
	ps = &buf[0];
	int read = wiringPiSPIDataRW(0, ps, 2048*16);
	send_flag = 1;
}

void UDP_SEND(void)
{
	sendto(s, ps+16, 2047*16, 0, (struct sockaddr*) &si_other, slen);
	send_flag = 0;
	fflush(stdout);
}

void die(char *s)
{
	perror(s);
	exit(1);
}
int main(){
	wiringPiSetup();
	
	//create a UDP socket
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) die("socket");
	
	// zero out the structure
	memset((char *) &si_me, 0, sizeof(si_me));
	
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(RPI_PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PC_PORT);
	inet_aton(ADDR_PC, &si_other.sin_addr);
	
	//bind socket to port
	if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1) die("bind");
	
	pinMode(24, OUTPUT);
	digitalWrite(24,LOW);
	ps = &buf[0];
	int err = wiringPiSPISetupMode(0,25000000, 0);
	int *p;

	wiringPiISR(PIN, INT_EDGE_RISING, &SPI_TXRX);
	delay(1000);
	digitalWrite(24,HIGH);
	fflush(stdout);
	
	while(1)
	{
		if(send_flag) 
		{
			UDP_SEND();
		}
	}
return 0;
}
