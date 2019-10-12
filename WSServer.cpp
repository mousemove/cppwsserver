
#include "WSServer.h"
int WSServer::initWS()
{
	int sockd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockd < 0) exit(EXIT_FAILURE);
	cout << _port << endl;
	struct sockaddr_in s_addr;
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = INADDR_ANY;
	s_addr.sin_port = htons(_port);

	if (bind(sockd, (struct sockaddr *)&s_addr, sizeof(s_addr)) < 0) {
		std::cout << "Socket bind error" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Socket initialization DONE" << sockd << std::endl;
	return sockd;
}
WSServer::WSServer(unsigned short int port,const string & addaddr) : _port(port),_addaddr(addaddr) {
	// TODO Auto-generated constructor stub
	_desc = initWS();

}

WSServer::~WSServer() {
	if(_desc != -1) {
		shutdown(_desc,2);
		close(_desc);
	}
}

void WSServer::operator()()
{


	listen(_desc, 5);
	while(1)
	{

		cout << "start l1sten ..." << endl;
		int client = accept(_desc, NULL, NULL);
		cout << "CLID:" << client << endl;
		char buffer[4096];
		recv(client,buffer,4096,0);
		string responce(buffer);
		string seckey =  preg_match("Sec-WebSocket-Key: ([A-Za-z0-9=\+\./\]{24})",responce,1);
		cout  << seckey << endl;
		if(responce.find(_addaddr) != string::npos   && responce.find("Upgrade: websocket") != string::npos && seckey != "")
		{
			string adkey = seckey+GUID;
			string shakey = sha1stringbase64(adkey);
			cout << seckey << "|" << adkey << "|" << shakey << endl;
			string answer = string("HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ")+shakey+string("\r\nSec-WebSocket-Protocol: chat\r\n\r\n");
			if(	send(client,answer.c_str(),answer.size(),0) !=0)
			{

				auto k =  std::thread([=]{
					bool processReadParted = false;//флаг для последующего склеивания сообщений
					vector<char> bufConcateRead(0);//вектор для последующей конкатенации - читаем полное сообщение если нет флага FIN
					while(1)
					{
						RFCHeader buff;
						memset(buff.buffer,0,sizeof(buff.buffer));
						auto rec_b = read(client, buff.buffer, RCFHEADSIZE);
						cout << "read:" << rec_b << " |  " << (unsigned short)buff.parted._firstbyte.asBIT.opcode  << endl;
						if(rec_b > 0)
						{

							if (rec_b < RCFHEADMINSIZE)//если клиент пишет в сокет меньше чем минимальный размер фрейма - отключаем его
							{
								closeClient(client);
								return;
							};
							if (find(_strategy->_supportedOperations.begin(),_strategy->_supportedOperations.end(), buff.parted._firstbyte.asBIT.opcode ) == _strategy->_supportedOperations.end() )
							{
								closeClient(client);
								return;
							};//если приходить неподдерживаемый опкод - отключаем клиента

							auto partitional_read = [&]() {
								RFCMessage message = translateHeader(buff);

								while(message.data.size() != message.writed)
								{
									cout << "dS" << endl;
									//cout << "message ds " << message.data.size() << "|"<< message.writed << "|" << rec_b << endl;
									auto rec_b = read(client, message.data.data()+message.writed, message.data.size() - message.writed);
									//cout << "read part:" << rec_b << endl;
									message.writed +=rec_b;
								}
								//cout << "before convert...." << endl;
								message.convert();
								//cout << "get text:" << message.data.data() << endl;
								bufConcateRead.insert(bufConcateRead.end(),message.data.begin(),message.data.end() );
								if (buff.parted._firstbyte.asBIT.FIN == 1) processReadParted = false;
								else processReadParted = true;
							};




							if (buff.parted._firstbyte.asBIT.opcode ==0)//продолжаем чтение
							{
								if(processReadParted == true)//если до этого мы начинали читать сообщение разделенное на несколько фреймов - продолжаем
									//если нет и нам пришло сообщение с 0 опкодом,  без 1-н опкода - то считаем такие фреймы от клиента некорректными и кикаем его
								{
									partitional_read();
								}
								else
								{
									closeClient(client);
									return;
								}
								RFCMessage message = translateHeader(buff);
							}
							else if (buff.parted._firstbyte.asBIT.opcode ==1)
							{
								partitional_read();
								/* если сообщение выставляется пакетами - выставляем флаг что продолжаем читать одно сообщение */
							}
							else if (buff.parted._firstbyte.asBIT.opcode ==8)
							{
								closeClient(client);
								return;
							}
							else
							{
								cout << "unknown or unsupported OPcode = " <<  buff.parted._firstbyte.asBIT.opcode << endl;
							}
						}
						else
						{
							cout << "no connection" << endl;
							shutdown(client,2);
							close(client);
							return;
						}

						if (processReadParted == false)
						{
							//операция с сообщение
							if(bufConcateRead.size() > 0)
							{
								auto pm = _strategy->processingMessage( bufConcateRead, seckey );
								//cout << string(bufConcateRead.begin(),bufConcateRead.end()) << endl;
								if(pm.first != -1)//если пришел положительный ответ на отправку
								{
									auto sm = makeSendMessage(pm);
									cout << "sm size = " << sm.size() << endl;
									auto res = send(client,sm.data(),sm.size(),0);
									cout << "send result  = " << res << endl;
								}
								else
								{
									cout << "empty question" << endl;
								}

							}
							bufConcateRead.resize(0);//очищаем
							bufConcateRead.shrink_to_fit();//очищаем блок памяти
						}
					}




				}
				);


				k.detach();
			}
			else
			{

				shutdown(client,2);
				close(client);
				return;
			}

		}
		else
		{
			cout << "bad request" << endl;
			shutdown(client,2);
			close(client);
		}
		//cout  << buffer << endl;

	}



}


RFCMessage WSServer::translateHeader( RFCHeader & buff)
{
	RFCMessage result;

	if (buff.parted._secondbyte.asBIT.length <= 125)
	{
		unsigned char messageLength = buff.parted._secondbyte.asBIT.length;
		result.data.resize(messageLength);

		unsigned char diff = messageLength > sizeof(buff.parted._additionaldata._shortmessage.messagePart) ?
				sizeof(buff.parted._additionaldata._shortmessage.messagePart) : messageLength;

		for(int i = 0;i<diff;i++)
		{
			result.data[i] = (buff.parted._additionaldata._shortmessage.messagePart[i]);
		}
		result.writed = diff;
		result.header = buff;
		return result;

	}
	else if(buff.parted._secondbyte.asBIT.length == 126 )
	{
		unsigned int messageLength = buff.parted._additionaldata._middlemessage.length[0];
		messageLength = messageLength << 8;
		messageLength = messageLength | buff.parted._additionaldata._middlemessage.length[1];
		result.data.resize(messageLength);

		for(int i = 0;i<6;i++)
		{
			result.data[i] = (buff.parted._additionaldata._middlemessage.messagePart[i]);
		}
		result.writed = 6;
		result.header = buff;
		return result;
	}
	else if(buff.parted._secondbyte.asBIT.length == 127 )
	{
		unsigned long int messageLength = buff.parted._additionaldata._longmessage.length[0];
		for(int i = 0;i < 8; i++)
		{
			messageLength |= (buff.parted._additionaldata._longmessage.length[i] << (56-i*8));
		}
		result.data.resize(messageLength);
		result.header = buff;
		result.writed = 0;
		return result;
	}
	return result;
}

vector<char> WSServer::makeSendMessage(pair<unsigned char,vector<char>> & input)
{
	vector<char> result;
	RFCHeader header;
	header.parted._firstbyte.asBIT.opcode = input.first;
	header.parted._firstbyte.asBIT.FIN = 1;//в ответах не поддерживается разбиение на части, сервер пишет данные одним фреймом
	header.parted._secondbyte.asBIT.MASK = 0;//сервер отправляет результат без маски

	if(input.second.size() < 126)
	{

		unsigned  char messageLength = input.second.size();
		header.parted._secondbyte.asBIT.length = messageLength;
		result.resize(2+messageLength);
		result[0] = header.parted._firstbyte.byte;
		result[1] = header.parted._secondbyte.byte;
		for(unsigned int i =  0; i < messageLength;i++)
		{
			result[i+2] = input.second[i];
		}
	}
	else if( input.second.size() < 65536)
	{
		unsigned  int messageLength = input.second.size();
		header.parted._secondbyte.asBIT.length = 126;
		result.resize(4+messageLength);
		result[0] = header.parted._firstbyte.byte;
		result[1] = header.parted._secondbyte.byte;
		result[2] = (messageLength & 0xFF00) >> 8;
		result[3] = (messageLength & 0xFF) ;
		for(unsigned int i =  0; i < messageLength;i++)
		{
			result[i+4] = input.second[i];
		}

	}
	else
	{
		unsigned long  int messageLength = input.second.size();
		header.parted._secondbyte.asBIT.length = 127;
		result.resize(10+messageLength);

		result[0] = header.parted._firstbyte.byte;
		result[1] = header.parted._secondbyte.byte;

		result[2] = (messageLength & 0xFF00000000000000) >> 56;
		result[3] = (messageLength & 0xFF000000000000) >> 48;
		result[4] = (messageLength & 0xFF0000000000) >> 40;
		result[5] = (messageLength & 0xFF00000000) >> 32;

		result[6] = (messageLength & 0xFF000000) >> 24;
		result[7] = (messageLength & 0xFF0000) >> 16;
		result[8] = (messageLength & 0xFF00) >> 8;
		result[9] = (messageLength & 0xFF) ;

		for(unsigned int i =  0; i < messageLength;i++)
		{
			result[i+10] = input.second[i];
		}

	}

	return result;

}

void WSServer::setStrategy(abstractStrategy * strategy)
{
	_strategy = strategy;
}

void WSServer::closeClient(unsigned int client)
{
	cout << "close client..." << endl;
	unsigned  char message[2] = {0b10001000, 0b0};
	send(client,message,2,0);
	shutdown(client,2);
	close(client);
}
