

#pragma once
#include "MyThread.h"


class RoomManagerThread : public MyThread
{
public:
	UxVoid InitThread() override;
	UxVoid ProcThread() override;
	UxVoid JoinThread() override;
};

