#include <winsock2.h>
#include <iostream>
#include <thread>
#include <set>
#include <map>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define PACKET_SIZE 1024

SOCKET sock, client_sock;

//Ŭ���̾�Ʈ ���� ���� ���� �� ������ ���� ����ü�� �����Ѵ�.
//Ŭ���̾�Ʈ�� ���� ��û���� �� ����(Accept)�ؾ���
SOCKADDR_IN client = {};


set<SOCKET> clientSock; //Ŀ�ؼǵ� ���ϵ��� ����ִ�. => ä�ù�
map<SOCKET, thread> clientThread;
map<int, string> clientNicknames; // ���� ���� Ŭ���̾�Ʈ��


//Client Socket�� �޴´�.
void ReceiveDataWithConnection(SOCKET socket)
{

	//Ŭ���̾�Ʈ�� ������ �����ϱ� ���� ��Ŷ ũ�⸸ŭ�� ���� ����
	char buffer[PACKET_SIZE] = { 0 };

	//���� �۾��� ���ؼ� ������ ���� ���� ������ ����(connection ����)
	while (!WSAGetLastError())
	{
		ZeroMemory(&buffer, PACKET_SIZE);

		//recv�� ����� �������κ��� �����͸� �޾ƿ´�. (connectionless �������κ��͵� �޾ƿ� �� �ִ�)
		if (!recv(socket, buffer, PACKET_SIZE, 0))
		{
			//�޼����� ���� �� ���ٰ� �Ǵ�
			break;
		}

		std::cout << buffer << "\n";

		//ä�ù��̱� ������ �ٸ� client�鿡�Ե� �� Ŭ���̾�Ʈ�� �����͸� ������� �Ѵ�.
		for (set<SOCKET>::iterator iter = clientSock.begin(); iter != clientSock.end(); iter++)
		{
			//�ٸ� Ŭ���̾�Ʈ���� ��ε�ĳ��Ʈ
			if (*iter != socket)
			{
				send(*iter, buffer, strlen(buffer), 0);
			}
		}
	}
}


//client�� ��û�� �޴� ���� receive
//Listen ���·� ����ϸ鼭 ������ ���Ͽ� ���� client�� ��û�̿��� accept ���ش�.
void ReceiveRequest()
{
	while (1)
	{
		int client_size = sizeof(client);

		//ZeroMemory : client�� client_size��ŭ �޸� �Ҵ�
		ZeroMemory(&client, client_size);


		//Ŭ���̾�Ʈ�� ��û ���� => Connection�� �����ȴ�.(����ȭ ���)
		//������ ���ε� ������ ����Ѵٴ� ���̴�.
		client_sock = accept(sock, (SOCKADDR*)&client, &client_size);

		//�����ϸ� ������ �����Ǿ� ��ȯ�ȴ�. => client_sock


		//���ϵ��� �����ϱ� ���� üũ�Ѵ�. => set�� ���ο� ���� �߰�
		if (clientSock.find(client_sock) == clientSock.end())
		{
			clientSock.insert(client_sock);
			//�ش� Ŭ���̾�Ʈ���� ������ ���� => Ŭ���̾�Ʈ���� ������ �����ش�.
			//���� �񵿱��۾��� ���� ������ ����
			clientThread[client_sock] = thread(ReceiveDataWithConnection, client_sock);
		}
	}

	//�޼��尡 ���� ������ ����Ѵ�. �ٷ� ���μ����� �������� �ʴ´�.
	for (set<SOCKET>::iterator iter = clientSock.begin(); iter != clientSock.end(); iter++)
	{
		clientThread[*iter].join();
	}
}

int main()
{
	//WSADATA ������ ���� ������ ����ü
	WSADATA wsa;

	//WSAStartup ������ ��Ĺ�� �ʱ�ȭ �ϴ� �Լ�
	WSAStartup(MAKEWORD(2, 2), &wsa);


	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	//SOCKADDR_IN : ��Ĺ �ּ� ����ü
	SOCKADDR_IN addr = {};

	//sin_family : �ּ� ü�踦 �����ϱ� ���� ����
	//AF_INET : IPv4 ���ͳ� ��������
	//�׻� �̰ɷ� �����ؾ��Ѵ�.
	addr.sin_family = AF_INET;

	//sin_port : ��Ʈ��ȣ�� �ǹ��ϴ� ����
	addr.sin_port = htons(4567);

	//sin_addr : ȣ��Ʈ IP�ּ�
	addr.sin_addr.s_addr = htonl(INADDR_ANY);


	//bind => local address�� ���ϰ� ���ε�(�ּ� ���� �߰�)
	bind(sock, (SOCKADDR*)&addr, sizeof(addr));

	//listen -> ������ �����û ��� ���·� �����
	//���⼭ 2��° ���ڴ� connectiono queue�� ���Ѵ�. ��, ��û ť�� ����Ǵ� ���� ���� ���Ѵٰ� ���� �ȴ�. 
	//SOMAXCONN = 0x7fffffff => 8����Ʈ ����.
	listen(sock, SOMAXCONN);

	//������ ����(Ŭ���̾�Ʈ�� ������ ���������� �޴� ������)
	thread proc(ReceiveRequest);
	char buffer[PACKET_SIZE] = { 0 };

	//���������� Ŭ���̾�Ʈ���� �޼����� ���� �� ����
	while (!WSAGetLastError())
	{
		//�޼��� ����
		std::cin >> buffer;
		string msg = "Notice : ";
		msg += buffer;
		for (set<SOCKET>::iterator iter = clientSock.begin(); iter != clientSock.end(); iter++)
		{
			send(*iter, msg.c_str(), strlen(msg.c_str()), 0);
		}
	}

	//������� ���������� ������� ����
	proc.join();


	//������ ����� Ŭ���̾�Ʈ�� ������ �ݰ� ����
	for (set<SOCKET>::iterator iter = clientSock.begin(); iter != clientSock.end(); iter++)
	{
		closesocket(*iter);
	}

	//������ ���ϵ� ����
	closesocket(sock);
	//����
	WSACleanup();
}