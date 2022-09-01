#include <WS2tcpip.h>
#include <iostream> 
#include <thread>
#include <winsock2.h> 

#pragma comment(lib, "ws2_32.lib")

//1. 소켓을 사용하기 위해서는 #include <winsock2.h> & #pragma comment(lib, "ws2_32.lib") 이 2개의 라인이 중요하다.
//다시말해서 WinSock2.h 기능을 사용하기 위해서는 ws2_32 라이브러리를 링크해줘야 한다.

using namespace std;

string nickName;

#define PACKET_SIZE 1024 //임의로 패킷사이즈를 정했다.

SOCKET sListen; //소켓

void proc_recv()
{
	char buffer[PACKET_SIZE] = {};
	string cmd;
	while (!WSAGetLastError())
	{
		ZeroMemory(&buffer, PACKET_SIZE);
		recv(sListen, buffer, PACKET_SIZE, 0);
		cmd = buffer;
		std::cout << buffer << "\n";
	}
}

int main()
{
	std::cout << "서버에서 사용할 닉네임을 입력해주세요!\n";
	std::cin >> nickName;

	//WSADATA 윈도우 소켓 구조체이다.
	//소켓의 config라고 보면 된다. (소켓 초기화 정보를 저장한다.)
	WSADATA wsa;


	//WSAStartup(소켓 버전(WORD), WSADATA 구조체 주소) => 윈도우 소캣을 초기화 하는 함수이다.
	//인자로 WORD 타입으로 들어가야 하므로 MAKEWORD 라는 매크로함수를 이용한다.
	//MAKEWORD : 매크로함수 (0x0202를 리턴한다)
	WSAStartup(MAKEWORD(2, 2), &wsa);

	//Q.버전이 달라지면 어떻게 될까???


	//소켓의 정보를 담아서 초기화를 하면 이제 소켓을 만들 수 있다.
	//소켓 자체는 핸들러 중 하나다. 커널오브젝트의 한 종류다.

	//socket(domain,type, protocol)
	//PF_INET : IPv4 인터넷 프로토콜을 사용
	//SOCK_STREAM : TCP/IP 프로토콜을 이용  => UDP는 SOCK_DGRAM 이라고 한다.
	//IPPROTO_TCP : TCP 프로토콜

	//미리 다 winsock2.h 에 지정되어있는 매크로들이다.
	sListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);



	//SOCKADDR_IN => 소켓 구성요소를 담아야 한다. 필수적으로 IP와 PORT 번호가 있다.
	//즉, endpoint를 어디다 두는지 여기서 정한다 (local or remote)
	SOCKADDR_IN addr = {};


	//sin_family : 주소 체계를 구분하기 위한 변수
	//AF_INET : IPv4 인터넷 프로토콜
	//다른 것도 있지만 docs에는 MUST BE AF_INET를 사용하란다.
	addr.sin_family = AF_INET; // 정의를 보면 for internetwork: UDP, TCP, etc.

	//왜 MUST BE일까?? => POSIX를 지켜야하기에 다른 OS에선 지켜진 POSIX API를 WINDOW에선 이 변수를 씀으로써 지킬 수 있다.


	//sin_port 포트설정 => 포트를 설정해야 해당 IP의 정확한 프로세스와 통신이 가능하다.
	//PORT 번호는 2바이트 안에서 표현할 수 있는 숫자로 정해야 하고 
	//기본으로 정해진 포트를 제외한 포트번호를 설정해야 한다.
	//htos (host to network short)의 약자
	addr.sin_port = htons(4567);

	//sin_addr : 호스트 IP주소 (로컬이다) 일반적으로 서버가 동작되는 컴퓨터의 IP 주소로 한다.
	//inet_addr은 string을 ipv4로 바꿔준다(IN_ADDR에 맞추어)
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//htonl은 32-bit 호스트 숫자를 TCP/IP네트워크 바이트오더(Big-Endian)에 맞게 변환시켜준다.
	//addr.sin_addr.s_addr = htonl(INADDR_ANY);



	//소켓을 생성했으면 접속 대기 상태로 만들어준다.
	while (1)
	{
		//connect : 원격 호스트에 대한 연결 => 서버 IP로의 커넥션 생성
		if (!connect(sListen, (SOCKADDR*)&addr, sizeof(addr)))
		{
			break;
		}
	}
	thread proc1(proc_recv);

	char buffer[PACKET_SIZE] = { 0 };

	while (!WSAGetLastError())
	{
		//소캣에러가 발생중이지 않을때
		//메세지 입력

		std::cin >> buffer;
		string msg = "";
		msg += nickName;
		msg += " : ";
		msg += buffer;

		send(sListen, msg.c_str(), strlen(msg.c_str()), 0);
	}
	proc1.join();

	closesocket(sListen);


	//WSAStartup 과 WSACleanup은 붙어다닌다.
	//이 사이에서 소켓을 활용하는 것이다. (생성자, 소멸자와 같은 느낌)
	WSACleanup();
}
