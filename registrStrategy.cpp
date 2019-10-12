
#include "registrStrategy.h"
#include "algorithm"
//Стратегия пример - получаем на вход строку и конвертируем её в верхний регистр
registrStrategy::registrStrategy() {
	// TODO Auto-generated constructor stub
	_supportedOperations = {0,1,8};//поддерживаем только эти оп коды
}

registrStrategy::~registrStrategy() {
	// TODO Auto-generated destructor stub
}


pair<unsigned char,vector<char>>  registrStrategy::processingMessage(const vector<char> & input,const string &seckey)
{
	vector<char> result(0);
	if(input.size() == 0) return make_pair(-1,result);
	string s = string(input.begin(),input.end());
	transform(s.begin(), s.end(),s.begin(), ::toupper);
	//cout << "out string before write to vector:" << s << "|" << s.size() << endl;
	result.insert(result.end(),s.begin(),s.end());
	result.insert(result.end(),'\0');
	return make_pair(1,result);
}
