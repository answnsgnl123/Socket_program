#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUF_SIZE 1024
#define OPSZ 4
#define RLT_SIZE 4

void error_handling(char *message);

int main(int argc, char* argv[])
{
	int sock;
	char opmsg[BUF_SIZE];
	int result, opnd_cnt, i;
	struct sockaddr_in serv_addr;

	char *host, **names, **addrs;
	struct hostnet *hostinfo;
	/*
	컴퓨터 loacl host를 받아와 ip byte 를 char[] 변수의 담았습니다.
	바로 실행하여 사용할수있습니다.
	
	*/
	if(argc == 1) {
		char myname[256];
		gethostname(myname,255);
		host = myname;
	} else
		host = argv[1];
	//구조체를 사용하여 내컴퓨터 아이피 도메인 host네임을 가져옵니다"
	hostinfo = gethostbyname(host);

	if(!hostinfo){
		fprintf(stderr,"Cannot get info for host %s\n",host);
		exit(1)
	}
	printf("Results for host %s:\n", host);
	printf("Name: %s\n", hostinfo->h_name);
	printf("domain Aliases:");
	
	while(*names){
		printf("%s",*names);
		names++;
	}
	printf("\n");
	if(hostinfo -> h_addrtype != AF_INET){
		fprintf(stderr,"NOt an IP HOST!");
	}
	//addrs 값인 127.0.1.1를 반환하여 실행할떄 argv값을 따로 넣어주지않아도 자동으로 localhost로 접속합니다.
	addrs = hostinfo -> h_addr_list;
	while(*addrs){
		printf(" %s",inet_ntoa(*(struct in_addr *) *addrs));
		addrs++;
	}
	printf("\n");
	sock=socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2])); //<< 포트만 설정해주면 사용가능했습니다.


	/*처음 소켓에 버퍼 사이즈를 1024설정하여 기회를 1byte 로보내고 
	1byte 를받아 ip를 char 127.0.1.1를 받아 1byte + 9byte를 socket 처음에 담아보냅니다.
	그러고 이제 받은 내용으로 게임을진행하는 server 쪽 로직이 수행하여 hit down up 값을 보내줍니다.
	hit 적중하면 hit 로직을 실행히여 소켓을 닫을지 말지 y or n으로 값을 받습니다.
	y누르면 서버 다신연결하여 게임을 진행했습니다 .
	이러한 로직이있었는데.. 5일간의 노력이 사라졌씁니다.. ㅠ.ㅠ
	*/
	if(connect(sock, (struct sockaddr*)&serv_addr, 	sizeof(serv_addr))==-1) 
		error_handling("connect() error!");

	fputs("Operand count: ", stdout);
	scanf("%d", &opnd_cnt);
	opmsg[0]=(char)opnd_cnt;
	
	for(i=0; i<opnd_cnt; i++) {
		printf("Operand %d: ", i+1);
		scanf("%d", (int*)&opmsg[i*OPSZ+1]);
	}

	fgetc(stdin);
	fputs("Operator: ", stdout);
	scanf("%c", &opmsg[opnd_cnt*OPSZ+1]);
	write(sock, opmsg, opnd_cnt*OPSZ+2);
	read(sock, &result, RLT_SIZE);					

	printf("Operation result: %d \n", result);

	close(sock);
	return 0;
}

/* 서버에서 hit down up 값을받아 처리했던 함수가있었던곳 ..*/
void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
