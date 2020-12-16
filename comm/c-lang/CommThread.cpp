// CommThread.cpp: implementation of the CCommThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommThread.h"

extern HWND hCommWnd;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

long ireadbyte;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommThread::CCommThread()
{
	m_bConnected = FALSE;

#if 0	// gyj :: com3(rs-485 제어) 기능 추가..
	//m_testRecv = FALSE;		// gyj :: 이 설정은 rs-485 발신후에 수신을 하는 기능으로 사용.. (즉 시험을 하는 보드의 설정.)
	m_testRecv = TRUE;		// gyj  :: 이 설정은 rs-485  수신&발신 thread 를 사용한다는 flag임. (rs-485 data를 받아서 다시 돌려주는 기능의 수행.) 
#endif
}

CCommThread::~CCommThread()
{

}


BOOL CCommThread::OpenPort(CString PortName, DWORD BaudRate)
{
	COMMTIMEOUTS	timeouts;
	DCB				dcb;
	DWORD			dwBaud;
	DWORD			dwThreadID;

	// overlapped structure 변수 초기화.
	dwBaud = BaudRate;
	m_osRead.Offset = 0;
	m_osRead.OffsetHigh = 0;
	//--> Read 이벤트 생성에 실패..
	if ( !(m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)) ) 	
	{
		
		return FALSE;
	}

	m_osWrite.Offset = 0;
	m_osWrite.OffsetHigh = 0;
	//--> Write 이벤트 생성에 실패..
	if (! (m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))
	{
		
		return FALSE;
	}
	
	m_hComm = CreateFile(PortName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0);

	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(PortName);
		return FALSE;
	}
	
	// EV_RXCHAR event 설정...데이터가 들어오면.. 수신 이벤트가 발생하게끔..
	SetCommMask(m_hComm, EV_RXCHAR);
	// InQueue, OutQueue 크기 설정.
	SetupComm(m_hComm, BUFF_SIZE, BUFF_SIZE);	
	// 포트 비우기.
	PurgeComm( m_hComm, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
	// 타임아웃 설정 잘 모르겠다.
	timeouts.ReadIntervalTimeout = 0xFFFFFFFF;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;	
	timeouts.WriteTotalTimeoutMultiplier = 2 * CBR_9600 / dwBaud;
	timeouts.WriteTotalTimeoutConstant = 0;
	
	SetCommTimeouts( m_hComm, &timeouts);

	// dcb 설정.... 포트의 실제적인..제어를 담당하는 DCB 구조체값 셋팅..
	dcb.DCBlength = sizeof(DCB);

	//--> 현재 설정된 값 중에서..
	GetCommState( m_hComm, &dcb);	
	
	//--> 보드레이트를 바꾸고..	
	dcb.BaudRate = dwBaud;	

	//--> Data 8 Bit
	dcb.ByteSize = 8;

	//--> Noparity
	dcb.Parity = NOPARITY;
	//dcb.Parity = EVENPARITY;
	//dcb.fParity = FALSE;

	//--> 1 Stop Bit
	dcb.StopBits = ONESTOPBIT;

#if 0	// gyj :: com3(rs-485 제어) 기능 추가..
	m_sPortName = PortName;

	Sleep(50);
	RETAILMSG(1,(TEXT("\t  m_sPortName=%s \n"),m_sPortName));
	if (m_sPortName == "COM3:")
	{
		dcb.fRtsControl = RTS_CONTROL_ENABLE;  // RTS_CONTROL_ENABLE , RTS_CONTROL_HANDSHAKE
		RETAILMSG(1,(TEXT("\t  COM3(rs-485 port) ==>  RTS_CONTROL_ENABLE \n")));
	}
	else
		dcb.fRtsControl = RTS_CONTROL_DISABLE;
#else
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
#endif

	//--> 포트를 재..설정값으로.. 설정해보고..
	if( !SetCommState( m_hComm, &dcb) )	
	{
		return FALSE;
	}
	// 포트 감시 쓰레드 생성.
	m_bConnected = TRUE;

	//--> 포트 감시 쓰레드 생성.
//#if 1	// gyj :: com3(rs-485 제어) 기능 추가..	:: send를 먼저 하는 프로세스는  recv감시 thread를 가동하면 안됨.   즉  data를 보낸후에  수신mode로 전환해서,  수신을 기다려야 한다.
//	if (m_sPortName == "COM3:" && !m_testRecv)
//		;
//	else
	{
		m_hThreadWatchComm = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)ThreadWatchComm, this, 0, &dwThreadID);

		if (! m_hThreadWatchComm)
		{
			AfxMessageBox( _T("Fali Thread Create"));		
		}
	}
//#endif
	return TRUE;
}

DWORD	ThreadWatchComm(CCommThread* pComm)
{
   DWORD           dwEvent;
   OVERLAPPED      os;
   BOOL            bOk = TRUE;
   BYTE            buff[2048];      // 읽기 버퍼
   DWORD           dwRead;			// 읽은 바이트수.
   BYTE            recvBuf[2048];
   DWORD           rLen;
 

   // Event, OS 설정.
   memset( &os, 0, sizeof(OVERLAPPED));
   
   //--> 이벤트 설정..
   if( !(os.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL)) )
   {
		bOk = FALSE;
   }

   //--> 이벤트 마스크..
#if 0	// gyj :: com3(rs-485 제어) 기능 추가..,   여기서는 입력을 기다리도록 해 놓아야 한다.
	if (pComm->m_sPortName == "COM3:")	
	{
		if( !SetCommMask( pComm->m_hComm, (EV_RXCHAR|EV_RXFLAG)) )
		{
			bOk = FALSE;
		}
	}
	else
	{
		if( !SetCommMask( pComm->m_hComm, EV_RXCHAR) )
		{
			bOk = FALSE;
		}
	}
#else
   if( !SetCommMask( pComm->m_hComm, EV_RXCHAR) )
   {
	   bOk = FALSE;
   }
#endif

   //--> 이벤트나..마스크 설정에 실패함..
   if( !bOk )
   {
		AfxMessageBox(_T("Error while creating ThreadWatchComm"));
		return FALSE;
   }
  
   while (pComm ->m_bConnected)		//포트가 연결되면 무한 루프에 들어감
   {
 		dwEvent = 0;

        // 포트에 읽을 거리가 올때까지 기다린다.
        WaitCommEvent( pComm->m_hComm, &dwEvent, NULL);
		RETAILMSG(1,(TEXT("  ThreadWatchComm() received EVT = EV_RXCHAR \n")));
	
	
		//--> 데이터가 수신되었다는 메세지가 발생하면..
        if ((dwEvent & EV_RXCHAR) == (EV_RXCHAR))
        {
            // 포트에서 읽을 수 있는 만큼 읽는다.
			//--> buff 에 받아놓고..
			rLen=0;
			do
			{
				dwRead = pComm->ReadComm( buff, 2048); //들어온 데이터 읽어 오기 
				SendMessage(hCommWnd, WM_COMM_READ, (WPARAM) dwRead, 0 );

//				if(BUFF_SIZE - pComm->m_QueueRead.GetSize() > (int)dwRead)
//				{
//					for( WORD i = 0; i < dwRead; i++ )
//					{
//						pComm->m_QueueRead.PutByte(buff[i]);//큐 버퍼에  들어온 데이터 넣기 
//						recvBuf[rLen++] = buff[i];
//					}
//				}
				//else
				//	AfxMessageBox(_T("buff full")); //큐버퍼의 크기를 초과하면 경고 메시지 보냄
			}while(dwRead);

			#if 0	// gyj :: com3(rs-485 제어) 기능 추가..
			RETAILMSG(1,(TEXT("\t  ThreadWatchComm() received byte=%d \n"),rLen));

			if (pComm->m_sPortName=="COM3:" && pComm->m_testRecv)		// 받은 data를 다시 보내준다.
			{
				Sleep(10);
				pComm->WriteComm(recvBuf, rLen);
				RETAILMSG(1,(TEXT("\t  pComm->WriteComm(size=%d) \n"),rLen));
			}
			#endif

//			SendMessage(hCommWnd, WM_COMM_READ, dwRead, 0 );//CSerialComDlg로 데이터가 들어왔다는 메시지를 보냄
		}
		Sleep(0);	// 받은 데이터를 화면에 보여줄 시간을 벌기 위해.
					// 데이터를 연속으로 받으면 cpu점유율이 100%가 되어 화면에 뿌려주는 작업이 잘 안되고. 결과적으로 
					// 큐 버퍼에 데이터가 쌓이게 됨

#if 0	// gyj :: com3(rs-485 제어) 기능 추가..,   여기서는 입력을 기다리도록 해 놓아야 한다.
		if (pComm->m_sPortName == "COM3:")	 
		{
			Sleep(10);
			if (EscapeCommFunction(pComm->m_hComm,	SETRTS))	//
			{
				RETAILMSG(1,(TEXT("\t  ThreadWatchComm() :: success  COM3(rs-485 port) ==>	EscapeCommFunction( SETRTS ) 4 receive msg.\n")));
			}
			else
			{
				AfxMessageBox(_T("ThreadWatchComm()::  EscapeCommFunction(SETRTS)"));
				break;
			}

			//--> 이벤트 마스크..
			if( !SetCommMask( pComm->m_hComm, EV_RXCHAR) )
			{
				AfxMessageBox(_T("Error while creating ThreadWatchComm"));
				break;
			}
		}
#endif
						


   }
   
  CloseHandle( os.hEvent);

   //--> 쓰레드 종료가 되겠죠?
   pComm->m_hThreadWatchComm = NULL;

   return TRUE;

}



void CQueue::Clear()
{
	m_iHead = m_iTail =0;
	memset(buff,0,BUFF_SIZE);
}
CQueue::CQueue()
{
	Clear();
	nowdata = 0;
}
int CQueue::GetSize()
{
	return (m_iHead - m_iTail + BUFF_SIZE) % BUFF_SIZE;
}
BOOL CQueue::PutByte(BYTE b)
{
	if(GetSize() == (BUFF_SIZE-1)) return FALSE;
	buff[m_iHead++] =b;
	m_iHead %= BUFF_SIZE;
	nowdata++;
	return TRUE;
}
BOOL CQueue::GetByte(BYTE *pb)
{
	if(GetSize() == 0) return FALSE;
	*pb = buff[m_iTail++];
	m_iTail %= BUFF_SIZE;
	nowdata--;
	return TRUE;
}




// 포트로부터 pBuff에 nToWrite만큼 읽는다.
// 실제로 읽혀진 Byte수를 리턴한다.
DWORD CCommThread::ReadComm(BYTE *pBuff, DWORD nToRead)
{
	DWORD	dwRead,dwError, dwErrorFlags;
	COMSTAT comstat;

	//--- system queue에 도착한 byte수만 미리 읽는다.
	ClearCommError( m_hComm, &dwErrorFlags, &comstat);

	//--> 시스템 큐에서 읽을 거리가 있으면..
	dwRead = comstat.cbInQue;

	if(dwRead > 0)
	{
		//--> 버퍼에 일단 읽어들이는데.. 만일..읽어들인값이 없다면..	
		if( !ReadFile( m_hComm, pBuff, nToRead, &dwRead, NULL) )
		{

			dwError = GetLastError();
		}	
		
	}
	//--> 실제 읽어들인 갯수를 리턴.
	return dwRead;
}

DWORD CCommThread::WriteComm(BYTE *pBuff, DWORD nToWrite)
{
	DWORD	dwWritten, dwError, dwErrorFlags;
	COMSTAT	comstat;

	//--> 포트가 연결되지 않은 상태이면..
	if( !m_bConnected )		
	{
		return 0;
	}

#if 0	// gyj :: com3(rs-485 제어) 기능 추가..
	if (m_sPortName == "COM3:")
	{
		EscapeCommFunction(  m_hComm,  CLRRTS);		//CLRRTS
		RETAILMSG(1,(TEXT("\t  COM3(rs-485 port) ==>  EscapeCommFunction( CLRRTS ) 4 send \n")));
		//Sleep(20);
	}
#endif

//RETAILMSG(1,(TEXT("m_bConnected True\r\n")));

	//--> 인자로 들어온 버퍼의 내용을 nToWrite 만큼 쓰고.. 쓴 갯수를.,dwWrite 에 넘김.
	if( !WriteFile( m_hComm, pBuff, nToWrite, &dwWritten, NULL))
	{
		//--> 아직 전송할 문자가 남았을 경우..
		/*
		if (GetLastError() == ERROR_IO_PENDING)
		{
			// 읽을 문자가 남아 있거나 전송할 문자가 남아 있을 경우 Overapped IO의
			// 특성에 따라 ERROR_IO_PENDING 에러 메시지가 전달된다.
			//timeouts에 정해준 시간만큼 기다려준다.
			while (! GetOverlappedResult( m_hComm, &m_osWrite, &dwWritten, TRUE))
			{
				dwError = GetLastError();
				if (dwError != ERROR_IO_INCOMPLETE)
				{
					ClearCommError( m_hComm, &dwErrorFlags, &comstat);
					break;
				}
			}
		}
		else
		{
			dwWritten = 0;
			ClearCommError( m_hComm, &dwErrorFlags, &comstat);
		}
		*/
		RETAILMSG(1,(TEXT("error\r\n")));
	}

//RETAILMSG(1,(TEXT("no error\r\n")));

	//--> 실제 포트로 쓰여진 갯수를 리턴..
	return dwWritten;
}


void CCommThread::ClosePort()
{
	#if 0	// gyj :: com3(rs-485 제어) 기능 추가..
	m_sPortName = " ";
	#endif

	//--> 연결되지 않았음.
	m_bConnected = FALSE;	

	//--> 마스크 해제..
	SetCommMask( m_hComm, 0);
	
	//--> 포트 비우기.
	PurgeComm( m_hComm,	PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
	
	CloseHandle(m_hComm);
	//--> 핸들 닫기
	
}
