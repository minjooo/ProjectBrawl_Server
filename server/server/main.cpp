

#include "Header.h"
#include "Server.h"


int main()
{
	Server* server = Server::GetInstance();
	server->Run();
}