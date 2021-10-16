#ifndef TRC_INCLUDE_TVM_FLOAT_H
#define TRC_INCLUDE_TVM_FLOAT_H

#include "base.h"
#include "../../include/cfg.h"

using namespace std;

class trcfloat : public trcobj
{
public:
	double value;

	void putline(ostream& out);

	~trcfloat();

	trcfloat();

	trcfloat(const double &value);

	INTOBJ to_int();

	STRINGOBJ to_string();

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
	int type = FLOAT_T;
};

#endif
