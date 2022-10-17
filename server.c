#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h> //time 램덤 값을 더욱더 램덤하게 하기위한 라이브러리

#define BUF_SIZE 1024

void error_handling(char *message);
void ClearBuffer(void); //버퍼 디버깅용 테스터 함수
int random_num(int* range); // 램덤넘버 변수를 만들어주는 함수
char game(int value, int* random_num,char* result); //업엔다운 로직이 진행되는 함수


int main(int argc, char *argv[]) {

	int serv_sock, clnt_sock;
	char message[BUF_SIZE];
	int str_len;
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;

	//접속자를 알기위해 ip를 저장하는변수
	char ipaddres;
	
	// 램덤값 범위지정
	int range = 100;

	int random_int = random_num(&range);

	//디버깅 용 합계 변수
	int resuit;
		
	int client_len;
	
	//디버깅 용 합계 변수
	long tmp_long =0;

	//아이피 를 저장하는 변수
	char ip_address;
	//환영문
	char welcome[] = "wlecome This a number: guessing game.";

	int value_s[100];
	
	//main 이아니라 custom function을 사용하기위해 메인 변수의 접근하기위한 변수
	char game_result;

	char* resultPtr;

	resultPtr = &game_result;

	//히트 포인터를 받아 clint resuit 와 같이보내는 변수
	char hit_info = 'h';
	char* hitPtr;

	hitPtr = &hit_info;

	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	//소켓 생성
	serv_sock=socket(PF_INET, SOCK_STREAM, 0); 
	if(serv_sock==-1)
		error_handling("socket() error");
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET; //ipv 4사용
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY); //host ip 
	serv_adr.sin_port=htons(atoi(argv[1]));//port number 빅엔디안

	// 바인드
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	// 리슨
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	/*사용자의 아이피를 먼저 받아 저장후 게임진행*/
	for(int i =0; i<5; i++){
		printf("server waiting..\n");
		//램덤 넘버 생성
		printf("A ramdom number: %d\n", random_int);

		clnt_adr_sz = sizeof(clnt_adr);
		//accept 함수	
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		if(clnt_sock == -1){
			error_handling("accept_error");
		}
		else {
			/*try 횟수와 client ip를 받아 BUF_SIZE ->9사용  try+127.0.1.1 size 9 읽음*/
			while((str_len = read(clnt_sock,message,9)) != 0){
			printf("Connect client :%d\n",i+1);
			//resuit = read(clnt_sock,ip_address, strlen(message));

			//ClearBuffer();
			//printf("resuit values %d\n", resuit);
			printf("IP: %s\n", message);
			read(clnt_sock, message, BUF_SIZE);
			/*사용자 게임 클라이언트 up and down value 를받아 game function 매개변수로 전달
				전달 값을 다처리후 return 으로 char value를 받음 hit ,up ,down 중 하나
				
				hit면 hit 로직사용 message resuit value와 hit value를 넣어 send 
				client 받으면 리턴값이 message로 날라와 남은 서브루틴 수행
			*/
			for(int i = 0; i <  message[0]; i++){
				
				//printf("msg: %d\n", message[i*4+1]);

				value_s[i]= (int)message[i*4+1];
				//printf("valus_ptr: %d \n",value_s[i]);
				*resultPtr = game(value_s[i],&random_int,&game_result);
				printf("status :%s\n",resultPtr);
				
			}
			printf("msg : %s\n", message);
			printf("msg len: %ld\n", strlen(message));
			   if(*resultPtr = hit_info){
			      message[BUF_SIZE] = game(value_s[i],&random_int,&game_result);
			      message[1] = *resultPtr; 
			      write(clnt_sock,message,strlen(message));
			   } else 
			write(clnt_sock,message,strlen(message));	
			}
		}
		close(clnt_sock);
	}
	close(serv_sock);
	return 0;
}
//버퍼 청소 확인 작업 디버깅 function
void ClearBuffer(void){
	while(getchar() != '\n');
}
// error handling
void error_handling(char *message) {
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
// random number create
int random_num(int* range){
	int random_int;

	srand(time(NULL));
	random_int = rand() % *range;

	return random_int;

}
// game logic 수행 
char game(int value,int* random_num,char* result){
	

	char a[] = "hit";
	char b[] = "up";
	char c[] = "down";

	


	printf("value_game:%d, random: %d, status:%d\n",value, *random_num,*result);
	if(*random_num == value){
      		printf("hit\n");
		result = a;
	    return *result;
	}
	else if(*random_num < value){
		printf("up...\n");
		result = b;
	    return *result;	
	}
	else if(*random_num > value){
		printf("down...\n");
		result = c;
	    return *result;	
	}	


}

