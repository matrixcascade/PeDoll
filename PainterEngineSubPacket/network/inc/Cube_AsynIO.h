#pragma  once
#include "Cube_Thread.h"

template<typename __IO,typename __I,typename __O>
class Cube_AsynIO :public Cube_Thread
{
public:
	Cube_AsynIO(){};
	virtual ~Cube_AsynIO()
	{
	}

	virtual BOOL   Initialize(__IO &info)				=0;
	virtual void   Recv(__I &in)						=0;
	virtual size_t Send(__O &Out)						=0;
	virtual void   Close()								=0;

protected:
private:
};