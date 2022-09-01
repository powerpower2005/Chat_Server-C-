#include <WS2tcpip.h>
#include <iostream> 
#include <thread>
#include <winsock2.h> 

#pragma comment(lib, "ws2_32.lib")

//1. ������ ����ϱ� ���ؼ��� #include <winsock2.h> & #pragma comment(lib, "ws2_32.lib") �� 2���� ������ �߿��ϴ�.
//�ٽø��ؼ� WinSock2.h ����� ����ϱ� ���ؼ��� ws2_32 ���̺귯���� ��ũ����� �Ѵ�.

using namespace std;

string nickName;

#define PACKET_SIZE 1024 //���Ƿ� ��Ŷ����� ���ߴ�.

SOCKET sListen; //����

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
	std::cout << "�������� ����� �г����� �Է����ּ���!\n";
	std::cin >> nickName;

	//WSADATA ������ ���� ����ü�̴�.
	//������ config��� ���� �ȴ�. (���� �ʱ�ȭ ������ �����Ѵ�.)
	WSADATA wsa;


	//WSAStartup(���� ����(WORD), WSADATA ����ü �ּ�) => ������ ��Ĺ�� �ʱ�ȭ �ϴ� �Լ��̴�.
	//���ڷ� WORD Ÿ������ ���� �ϹǷ� MAKEWORD ��� ��ũ���Լ��� �̿��Ѵ�.
	//MAKEWORD : ��ũ���Լ� (0x0202�� �����Ѵ�)
	WSAStartup(MAKEWORD(2, 2), &wsa);

	//Q.������ �޶����� ��� �ɱ�???


	//������ ������ ��Ƽ� �ʱ�ȭ�� �ϸ� ���� ������ ���� �� �ִ�.
	//���� ��ü�� �ڵ鷯 �� �ϳ���. Ŀ�ο�����Ʈ�� �� ������.

	//socket(domain,type, protocol)
	//PF_INET : IPv4 ���ͳ� ���������� ���
	//SOCK_STREAM : TCP/IP ���������� �̿�  => UDP�� SOCK_DGRAM �̶�� �Ѵ�.
	//IPPROTO_TCP : TCP ��������

	//�̸� �� winsock2.h �� �����Ǿ��ִ� ��ũ�ε��̴�.
	sListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);



	//SOCKADDR_IN => ���� ������Ҹ� ��ƾ� �Ѵ�. �ʼ������� IP�� PORT ��ȣ�� �ִ�.
	//��, endpoint�� ���� �δ��� ���⼭ ���Ѵ� (local or remote)
	SOCKADDR_IN addr = {};


	//sin_family : �ּ� ü�踦 �����ϱ� ���� ����
	//AF_INET : IPv4 ���ͳ� ��������
	//�ٸ� �͵� ������ docs���� MUST BE AF_INET�� ����϶���.
	addr.sin_family = AF_INET; // ���Ǹ� ���� for internetwork: UDP, TCP, etc.

	//�� MUST BE�ϱ�?? => POSIX�� ���Ѿ��ϱ⿡ �ٸ� OS���� ������ POSIX API�� WINDOW���� �� ������ �����ν� ��ų �� �ִ�.


	//sin_port ��Ʈ���� => ��Ʈ�� �����ؾ� �ش� IP�� ��Ȯ�� ���μ����� ����� �����ϴ�.
	//PORT ��ȣ�� 2����Ʈ �ȿ��� ǥ���� �� �ִ� ���ڷ� ���ؾ� �ϰ� 
	//�⺻���� ������ ��Ʈ�� ������ ��Ʈ��ȣ�� �����ؾ� �Ѵ�.
	//htos (host to network short)�� ����
	addr.sin_port = htons(4567);

	//sin_addr : ȣ��Ʈ IP�ּ� (�����̴�) �Ϲ������� ������ ���۵Ǵ� ��ǻ���� IP �ּҷ� �Ѵ�.
	//inet_addr�� string�� ipv4�� �ٲ��ش�(IN_ADDR�� ���߾�)
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//htonl�� 32-bit ȣ��Ʈ ���ڸ� TCP/IP��Ʈ��ũ ����Ʈ����(Big-Endian)�� �°� ��ȯ�����ش�.
	//addr.sin_addr.s_addr = htonl(INADDR_ANY);



	//������ ���������� ���� ��� ���·� ������ش�.
	while (1)
	{
		//connect : ���� ȣ��Ʈ�� ���� ���� => ���� IP���� Ŀ�ؼ� ����
		if (!connect(sListen, (SOCKADDR*)&addr, sizeof(addr)))
		{
			break;
		}
	}
	thread proc1(proc_recv);

	char buffer[PACKET_SIZE] = { 0 };

	while (!WSAGetLastError())
	{
		//��Ĺ������ �߻������� ������
		//�޼��� �Է�

		std::cin >> buffer;
		string msg = "";
		msg += nickName;
		msg += " : ";
		msg += buffer;

		send(sListen, msg.c_str(), strlen(msg.c_str()), 0);
	}
	proc1.join();

	closesocket(sListen);


	//WSAStartup �� WSACleanup�� �پ�ٴѴ�.
	//�� ���̿��� ������ Ȱ���ϴ� ���̴�. (������, �Ҹ��ڿ� ���� ����)
	WSACleanup();
}
