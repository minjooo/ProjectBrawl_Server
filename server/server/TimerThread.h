

#pragma once
#include "MyThread.h"

class TimerThread final : public MyThread
{
public:
	UxVoid InitThread() override;
	UxVoid ProcThread() override;
	UxVoid JoinThread() override;
};

