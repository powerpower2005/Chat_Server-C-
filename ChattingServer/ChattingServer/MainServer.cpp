#include <winsock2.h>
#include <iostream>
#include <thread>
#include <set>
#include <map>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define PACKET_SIZE 1024

SOCKET sock, client_sock;

//클라이언트 측의 소켓 생성 및 정보를 담을 구조체를 생성한다.
//클라이언트가 접속 요청했을 때 승인(Accept)해야함
SOCKADDR_IN client = {};


set<SOCKET> clientSock; //커넥션된 소켓들이 들어있다. => 채팅방
map<SOCKET, thread> clientThread;
map<int, string> clientNicknames; // 참여 중인 클라이언트들


//Client Socket을 받는다.
void ReceiveDataWithConnection(SOCKET socket)
{

	//클라이언트측 정보를 수신하기 위한 패킷 크기만큼의 버퍼 생성
	char buffer[PACKET_SIZE] = { 0 };

	//소켓 작업에 대해서 에러가 뜨지 않을 때까지 지속(connection 지속)
	while (!WSAGetLastError())
	{
		ZeroMemory(&buffer, PACKET_SIZE);

		//recv는 연결된 소켓으로부터 데이터를 받아온다. (connectionless 소켓으로부터도 받아올 수 있다)
		if (!recv(socket, buffer, PACKET_SIZE, 0))
		{
			//메세지를 받을 수 없다고 판단
			break;
		}

		std::cout << buffer << "\n";

		//채팅방이기 때문에 다른 client들에게도 이 클라이언트의 데이터를 보내줘야 한다.
		for (set<SOCKET>::iterator iter = clientSock.begin(); iter != clientSock.end(); iter++)
		{
			//다른 클라이언트에게 브로드캐스트
			if (*iter != socket)
			{
				send(*iter, buffer, strlen(buffer), 0);
			}
		}
	}
}


//client의 요청을 받는 역할 receive
//Listen 상태로 대기하면서 서버의 소켓에 대해 client가 요청이오면 accept 해준다.
void ReceiveRequest()
{
	while (1)
	{
		int client_size = sizeof(client);

		//ZeroMemory : client에 client_size만큼 메모리 할당
		ZeroMemory(&client, client_size);


		//클라이언트의 요청 승인 => Connection이 생성된다.(동기화 방식)
		//접속이 승인될 때까지 대기한다는 말이다.
		client_sock = accept(sock, (SOCKADDR*)&client, &client_size);

		//승인하면 소켓이 생성되어 반환된다. => client_sock


		//소켓들을 관리하기 위해 체크한다. => set에 새로운 소켓 추가
		if (clientSock.find(client_sock) == clientSock.end())
		{
			clientSock.insert(client_sock);
			//해당 클라이언트에게 스레드 생성 => 클라이언트마다 소켓을 열어준다.
			//또한 비동기작업을 위한 스레드 생성
			clientThread[client_sock] = thread(ReceiveDataWithConnection, client_sock);
		}
	}

	//메서드가 끝날 때까지 대기한다. 바로 프로세스를 종료하지 않는다.
	for (set<SOCKET>::iterator iter = clientSock.begin(); iter != clientSock.end(); iter++)
	{
		clientThread[*iter].join();
	}
}

int main()
{
	//WSADATA 윈도우 소켓 데이터 구조체
	WSADATA wsa;

	//WSAStartup 윈도우 소캣을 초기화 하는 함수
	WSAStartup(MAKEWORD(2, 2), &wsa);


	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	//SOCKADDR_IN : 소캣 주소 구조체
	SOCKADDR_IN addr = {};

	//sin_family : 주소 체계를 구분하기 위한 변수
	//AF_INET : IPv4 인터넷 프로토콜
	//항상 이걸로 설정해야한다.
	addr.sin_family = AF_INET;

	//sin_port : 포트번호를 의미하는 변수
	addr.sin_port = htons(4567);

	//sin_addr : 호스트 IP주소
	addr.sin_addr.s_addr = htonl(INADDR_ANY);


	//bind => local address를 소켓과 바인딩(주소 정보 추가)
	bind(sock, (SOCKADDR*)&addr, sizeof(addr));

	//listen -> 소켓을 연결요청 대기 상태로 만든다
	//여기서 2번째 인자는 connectiono queue를 말한다. 즉, 요청 큐에 저장되는 소켓 수를 말한다고 보면 된다. 
	//SOMAXCONN = 0x7fffffff => 8바이트 숫자.
	listen(sock, SOMAXCONN);

	//스레드 생성(클라이언트의 응답을 지속적으로 받는 스레드)
	thread proc(ReceiveRequest);
	char buffer[PACKET_SIZE] = { 0 };

	//서버에서도 클라이언트에게 메세지를 보낼 수 있음
	while (!WSAGetLastError())
	{
		//메세지 전송
		std::cin >> buffer;
		string msg = "Notice : ";
		msg += buffer;
		for (set<SOCKET>::iterator iter = clientSock.begin(); iter != clientSock.end(); iter++)
		{
			send(*iter, msg.c_str(), strlen(msg.c_str()), 0);
		}
	}

	//스레드는 끝날때까지 종료되지 않음
	proc.join();


	//오류가 생기면 클라이언트의 소켓을 닫고 종료
	for (set<SOCKET>::iterator iter = clientSock.begin(); iter != clientSock.end(); iter++)
	{
		closesocket(*iter);
	}

	//서버의 소켓도 닫음
	closesocket(sock);
	//종료
	WSACleanup();
}