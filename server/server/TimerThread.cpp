

#include "Header.h"
#include "TimerThread.h"
#include "Server.h"


UxVoid TimerThread::InitThread()
{
	mythread = std::thread( [&] () {TimerThread::ProcThread(); } );
}


UxVoid TimerThread::ProcThread()
{
	while ( true )
	{
		while ( true == Server::GetInstance()->m_timerQueue.empty() )
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
		}

		EVENT ev;
		if ( true == Server::GetInstance()->m_timerQueue.try_pop( ev ) )
		{
			if ( std::chrono::high_resolution_clock::now() < ev.wakeup_time )
			{
				std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
				Server::GetInstance()->m_timerQueue.push( ev );
				continue;
			}
			EVENT proc_ev = ev;

			OVER_EX* over_ex = new OVER_EX;

			if ( proc_ev.event_type == EEventType::TICK )
			{
				over_ex->ev_type = EEventType::TICK;
				*( EVENTINFO* )over_ex->net_buf = proc_ev.info;
			}
			else if ( proc_ev.event_type == EEventType::INVINCIBLEDONE )
			{
				over_ex->ev_type = EEventType::INVINCIBLEDONE;
				*( EVENTINFO* )over_ex->net_buf = proc_ev.info;
			}
			else if ( proc_ev.event_type == EEventType::RESETCOOLTIME )
			{
				over_ex->ev_type = EEventType::RESETCOOLTIME;
				*( EVENTINFO* )over_ex->net_buf = proc_ev.info;
			}
			else if ( proc_ev.event_type == EEventType::SKILLDELAY )
			{
				over_ex->ev_type = EEventType::SKILLDELAY;
				*( EVENTINFO* )over_ex->net_buf = proc_ev.info;
			}
			else
			{
				std::cout << "Unknown Event Type Error! \n";
				while ( true );
			}

			PostQueuedCompletionStatus( Server::GetInstance()->m_iocp, 1, proc_ev.id, &over_ex->over );
		}
	}
}

UxVoid TimerThread::JoinThread()
{
	mythread.join();
}