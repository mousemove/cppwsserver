

#ifndef ABSTRACTSTRATEGY_H_
#define ABSTRACTSTRATEGY_H_
#include "additional.h"
class abstractStrategy {
public:
	abstractStrategy();
	vector<char> _supportedOperations;//поддерживаемые ОП коды RFC 6445

	virtual ~abstractStrategy();
	virtual pair<unsigned char,vector<char>>  processingMessage( const vector<char> & input,const string &seckey) = 0;
	//формат ответа - оп код, вектор char с данными
};

#endif /* ABSTRACTSTRATEGY_H_ */
