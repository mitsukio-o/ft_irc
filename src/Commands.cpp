// B's part

#include "Server.hpp"

const Server::Command	Server::_commands[] = {
	{ 0, 0, false, 0 }
};

void	Server::execute(Client& client, const std::string& line)
{
	client.push("echo: " + line);	// 受信チェック用の仮実装。B-4 で消す
}

void	Server::welcome(Client& client)	{ (void)client; }

void	Server::cmdCap(Client& c, const Args& a)		{ (void)c; (void)a; }
void	Server::cmdPass(Client& c, const Args& a)	{ (void)c; (void)a; }
void	Server::cmdNick(Client& c, const Args& a)	{ (void)c; (void)a; }
void	Server::cmdUser(Client& c, const Args& a)	{ (void)c; (void)a; }
void	Server::cmdPing(Client& c, const Args& a)	{ (void)c; (void)a; }
void	Server::cmdPong(Client& c, const Args& a)	{ (void)c; (void)a; }
void	Server::cmdQuit(Client& c, const Args& a)	{ (void)c; (void)a; }
void	Server::cmdJoin(Client& c, const Args& a)	{ (void)c; (void)a; }
void	Server::cmdPart(Client& c, const Args& a)	{ (void)c; (void)a; }
void	Server::cmdTopic(Client& c, const Args& a)	{ (void)c; (void)a; }
void	Server::cmdInvite(Client& c, const Args& a)	{ (void)c; (void)a; }
void	Server::cmdKick(Client& c, const Args& a)	{ (void)c; (void)a; }
void	Server::cmdMode(Client& c, const Args& a)	{ (void)c; (void)a; }
void	Server::cmdPrivmsg(Client& c, const Args& a)	{ (void)c; (void)a; }
void	Server::cmdNotice(Client& c, const Args& a)	{ (void)c; (void)a; }
