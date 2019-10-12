
#ifndef WSSERVER_H_
#define WSSERVER_H_
#include "additional.h"
#include "abstractStrategy.h"
#include "registrStrategy.h"
using namespace std;
#define RCFHEADMINSIZE 6
#define RCFHEADSIZE 14
class WSServer {
public:
	unsigned short int _port;//порт
	int _desc = -1;//дескриптор сокета
	string _addaddr = "";//дополнение к адресу для "фильтрации" запросов
	const string GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";//guid строка из протоколов вебсокетов
	WSServer(unsigned short int port,const string & addaddr);
	abstractStrategy * _strategy;//стратегия обработки сообщений
	void setStrategy(abstractStrategy * strategy);//функция выставки стратегии
	int initWS();//инициализация сокета
	void operator()();//основной поток для приема новых клиентов
	RFCMessage translateHeader( RFCHeader & buff);//функция трансляции заголовка входного RFC 6455 сообщения
	vector<char> makeSendMessage(pair<unsigned char,vector<char>> & input);//функция создания RFC 6455 сообщения по вхондому вектору char'ов
	void closeClient(unsigned int client);

	virtual ~WSServer();
};

#endif /* WSSERVER_H_ */
