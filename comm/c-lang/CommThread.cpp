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

#if 0	// gyj :: com3(rs-485 ����) ��� �߰�..
	//m_testRecv = FALSE;		// gyj :: �� ������ rs-485 �߽��Ŀ� ������ �ϴ� ������� ���.. (�� ������ �ϴ� ������ ����.)
	m_testRecv = TRUE;		// gyj  :: �� ������ rs-485  ����&�߽� thread �� ����Ѵٴ� flag��. (rs-485 data�� �޾Ƽ� �ٽ� �����ִ� ����� ����.) 
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

	// overlapped structure ���� �ʱ�ȭ.
	dwBaud = BaudRate;
	m_osRead.Offset = 0;
	m_osRead.OffsetHigh = 0;
	//--> Read �̺�Ʈ ������ ����..
	if ( !(m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)) ) 	
	{
		
		return FALSE;
	}

	m_osWrite.Offset = 0;
	m_osWrite.OffsetHigh = 0;
	//--> Write �̺�Ʈ ������ ����..
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
	
	// EV_RXCHAR event ����...�����Ͱ� ������.. ���� �̺�Ʈ�� �߻��ϰԲ�..
	SetCommMask(m_hComm, EV_RXCHAR);
	// InQueue, OutQueue ũ�� ����.
	SetupComm(m_hComm, BUFF_SIZE, BUFF_SIZE);	
	// ��Ʈ ����.
	PurgeComm( m_hComm, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
	// Ÿ�Ӿƿ� ���� �� �𸣰ڴ�.
	timeouts.ReadIntervalTimeout = 0xFFFFFFFF;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;	
	timeouts.WriteTotalTimeoutMultiplier = 2 * CBR_9600 / dwBaud;
	timeouts.WriteTotalTimeoutConstant = 0;
	
	SetCommTimeouts( m_hComm, &timeouts);

	// dcb ����.... ��Ʈ�� ��������..��� ����ϴ� DCB ����ü�� ����..
	dcb.DCBlength = sizeof(DCB);

	//--> ���� ������ �� �߿���..
	GetCommState( m_hComm, &dcb);	
	
	//--> ���巹��Ʈ�� �ٲٰ�..	
	dcb.BaudRate = dwBaud;	

	//--> Data 8 Bit
	dcb.ByteSize = 8;

	//--> Noparity
	dcb.Parity = NOPARITY;
	//dcb.Parity = EVENPARITY;
	//dcb.fParity = FALSE;

	//--> 1 Stop Bit
	dcb.StopBits = ONESTOPBIT;

#if 0	// gyj :: com3(rs-485 ����) ��� �߰�..
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

	//--> ��Ʈ�� ��..����������.. �����غ���..
	if( !SetCommState( m_hComm, &dcb) )	
	{
		return FALSE;
	}
	// ��Ʈ ���� ������ ����.
	m_bConnected = TRUE;

	//--> ��Ʈ ���� ������ ����.
//#if 1	// gyj :: com3(rs-485 ����) ��� �߰�..	:: send�� ���� �ϴ� ���μ�����  recv���� thread�� �����ϸ� �ȵ�.   ��  data�� �����Ŀ�  ����mode�� ��ȯ�ؼ�,  ������ ��ٷ��� �Ѵ�.
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
   BYTE            buff[2048];      // �б� ����
   DWORD           dwRead;			// ���� ����Ʈ��.
   BYTE            recvBuf[2048];
   DWORD           rLen;
 

   // Event, OS ����.
   memset( &os, 0, sizeof(OVERLAPPED));
   
   //--> �̺�Ʈ ����..
   if( !(os.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL)) )
   {
		bOk = FALSE;
   }

   //--> �̺�Ʈ ����ũ..
#if 0	// gyj :: com3(rs-485 ����) ��� �߰�..,   ���⼭�� �Է��� ��ٸ����� �� ���ƾ� �Ѵ�.
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

   //--> �̺�Ʈ��..����ũ ������ ������..
   if( !bOk )
   {
		AfxMessageBox(_T("Error while creating ThreadWatchComm"));
		return FALSE;
   }
  
   while (pComm ->m_bConnected)		//��Ʈ�� ����Ǹ� ���� ������ ��
   {
 		dwEvent = 0;

        // ��Ʈ�� ���� �Ÿ��� �ö����� ��ٸ���.
        WaitCommEvent( pComm->m_hComm, &dwEvent, NULL);
		RETAILMSG(1,(TEXT("  ThreadWatchComm() received EVT = EV_RXCHAR \n")));
	
	
		//--> �����Ͱ� ���ŵǾ��ٴ� �޼����� �߻��ϸ�..
        if ((dwEvent & EV_RXCHAR) == (EV_RXCHAR))
        {
            // ��Ʈ���� ���� �� �ִ� ��ŭ �д´�.
			//--> buff �� �޾Ƴ���..
			rLen=0;
			do
			{
				dwRead = pComm->ReadComm( buff, 2048); //���� ������ �о� ���� 
				SendMessage(hCommWnd, WM_COMM_READ, (WPARAM) dwRead, 0 );

//				if(BUFF_SIZE - pComm->m_QueueRead.GetSize() > (int)dwRead)
//				{
//					for( WORD i = 0; i < dwRead; i++ )
//					{
//						pComm->m_QueueRead.PutByte(buff[i]);//ť ���ۿ�  ���� ������ �ֱ� 
//						recvBuf[rLen++] = buff[i];
//					}
//				}
				//else
				//	AfxMessageBox(_T("buff full")); //ť������ ũ�⸦ �ʰ��ϸ� ��� �޽��� ����
			}while(dwRead);

			#if 0	// gyj :: com3(rs-485 ����) ��� �߰�..
			RETAILMSG(1,(TEXT("\t  ThreadWatchComm() received byte=%d \n"),rLen));

			if (pComm->m_sPortName=="COM3:" && pComm->m_testRecv)		// ���� data�� �ٽ� �����ش�.
			{
				Sleep(10);
				pComm->WriteComm(recvBuf, rLen);
				RETAILMSG(1,(TEXT("\t  pComm->WriteComm(size=%d) \n"),rLen));
			}
			#endif

//			SendMessage(hCommWnd, WM_COMM_READ, dwRead, 0 );//CSerialComDlg�� �����Ͱ� ���Դٴ� �޽����� ����
		}
		Sleep(0);	// ���� �����͸� ȭ�鿡 ������ �ð��� ���� ����.
					// �����͸� �������� ������ cpu�������� 100%�� �Ǿ� ȭ�鿡 �ѷ��ִ� �۾��� �� �ȵǰ�. ��������� 
					// ť ���ۿ� �����Ͱ� ���̰� ��

#if 0	// gyj :: com3(rs-485 ����) ��� �߰�..,   ���⼭�� �Է��� ��ٸ����� �� ���ƾ� �Ѵ�.
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

			//--> �̺�Ʈ ����ũ..
			if( !SetCommMask( pComm->m_hComm, EV_RXCHAR) )
			{
				AfxMessageBox(_T("Error while creating ThreadWatchComm"));
				break;
			}
		}
#endif
						


   }
   
  CloseHandle( os.hEvent);

   //--> ������ ���ᰡ �ǰ���?
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




// ��Ʈ�κ��� pBuff�� nToWrite��ŭ �д´�.
// ������ ������ Byte���� �����Ѵ�.
DWORD CCommThread::ReadComm(BYTE *pBuff, DWORD nToRead)
{
	DWORD	dwRead,dwError, dwErrorFlags;
	COMSTAT comstat;

	//--- system queue�� ������ byte���� �̸� �д´�.
	ClearCommError( m_hComm, &dwErrorFlags, &comstat);

	//--> �ý��� ť���� ���� �Ÿ��� ������..
	dwRead = comstat.cbInQue;

	if(dwRead > 0)
	{
		//--> ���ۿ� �ϴ� �о���̴µ�.. ����..�о���ΰ��� ���ٸ�..	
		if( !ReadFile( m_hComm, pBuff, nToRead, &dwRead, NULL) )
		{

			dwError = GetLastError();
		}	
		
	}
	//--> ���� �о���� ������ ����.
	return dwRead;
}

DWORD CCommThread::WriteComm(BYTE *pBuff, DWORD nToWrite)
{
	DWORD	dwWritten, dwError, dwErrorFlags;
	COMSTAT	comstat;

	//--> ��Ʈ�� ������� ���� �����̸�..
	if( !m_bConnected )		
	{
		return 0;
	}

#if 0	// gyj :: com3(rs-485 ����) ��� �߰�..
	if (m_sPortName == "COM3:")
	{
		EscapeCommFunction(  m_hComm,  CLRRTS);		//CLRRTS
		RETAILMSG(1,(TEXT("\t  COM3(rs-485 port) ==>  EscapeCommFunction( CLRRTS ) 4 send \n")));
		//Sleep(20);
	}
#endif

//RETAILMSG(1,(TEXT("m_bConnected True\r\n")));

	//--> ���ڷ� ���� ������ ������ nToWrite ��ŭ ����.. �� ������.,dwWrite �� �ѱ�.
	if( !WriteFile( m_hComm, pBuff, nToWrite, &dwWritten, NULL))
	{
		//--> ���� ������ ���ڰ� ������ ���..
		/*
		if (GetLastError() == ERROR_IO_PENDING)
		{
			// ���� ���ڰ� ���� �ְų� ������ ���ڰ� ���� ���� ��� Overapped IO��
			// Ư���� ���� ERROR_IO_PENDING ���� �޽����� ���޵ȴ�.
			//timeouts�� ������ �ð���ŭ ��ٷ��ش�.
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

	//--> ���� ��Ʈ�� ������ ������ ����..
	return dwWritten;
}


void CCommThread::ClosePort()
{
	#if 0	// gyj :: com3(rs-485 ����) ��� �߰�..
	m_sPortName = " ";
	#endif

	//--> ������� �ʾ���.
	m_bConnected = FALSE;	

	//--> ����ũ ����..
	SetCommMask( m_hComm, 0);
	
	//--> ��Ʈ ����.
	PurgeComm( m_hComm,	PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
	
	CloseHandle(m_hComm);
	//--> �ڵ� �ݱ�
	
}
