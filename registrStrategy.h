
#ifndef REGISTRSTRATEGY_H_
#define REGISTRSTRATEGY_H_

#include "abstractStrategy.h"

class registrStrategy: public abstractStrategy {
public:
	registrStrategy();
	virtual ~registrStrategy();
	pair<unsigned char,vector<char>>  processingMessage( const vector<char> & input,const string &seckey);
};

#endif /* REGISTRSTRATEGY_H_ */
