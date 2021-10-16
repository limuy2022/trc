#ifndef TRC_INCLUDE_TVM_INT_H
#define TRC_INCLUDE_TVM_INT_H

#include "base.h"
#include "../../include/cfg.h"

using namespace std;

class trcint : public trcobj
{
public:
	int value = 0;

	void putline(ostream &out);

	~trcint();

	trcint();

	trcint(int value);

	STRINGOBJ to_string();

	FLOATOBJ to_float();

	INTOBJ to_bool();

	INTOBJ operator==(OBJ value_i);

	INTOBJ operator!=(OBJ value_i);
	
	INTOBJ operator<(OBJ value_i);
	
	INTOBJ operator>(OBJ value_i);
	
	INTOBJ operator<=(OBJ value_i);
	
	INTOBJ operator>=(OBJ value_i);
	
	OBJ operator+(OBJ value_i);
	
	OBJ operator-(OBJ value_i);
	
	OBJ operator*(OBJ value_i);
	
	OBJ operator/(OBJ value_i);
	
	OBJ operator%(OBJ value_i);
	
	OBJ pow_(OBJ value_i);
	
	OBJ zdiv(OBJ value_i);
	
	INTOBJ operator!();
	
	INTOBJ operator&&(OBJ value_i);

	INTOBJ operator||(OBJ value_i);

	int& gettype();

	void delete_();
private:
	// 整型标记类型
	int type = INT_T;
};

#endif
