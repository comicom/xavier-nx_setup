// CommThread.h: interface for the CCommThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMTHREAD_H__E90CA7C3_844A_433E_88C0_6B6348B0ADDB__INCLUDED_)
#define AFX_COMMTHREAD_H__E90CA7C3_844A_433E_88C0_6B6348B0ADDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define BUFF_SIZE			4096
#define WM_COMM_READ (WM_USER +1)


class CQueue
{
public:
	unsigned char buff[BUFF_SIZE];//큐 버퍼 사이즈는 4192로 만듬 
	int m_iHead, m_iTail;//큐 버퍼에 데이터를 넣고 빼 올때 사용할 변수 
	CQueue();
	void Clear();//버퍼를 초기화 시킴
	int GetSize();//현재 버퍼에 들어있는 데이터의 size를 리턴
	BOOL PutByte(unsigned char b);//큐버퍼에 1바이트를 넣음
	BOOL GetByte(unsigned char *pb);//큐버퍼에서 1바이트를 빼 옴
	int nowdata ;
	int GetNow(){return nowdata;}
};

class CCommThread  
{
public:
	CCommThread();
	virtual ~CCommThread();
	BOOL		OpenPort(CString PortName,DWORD BaudRate);
#if 1	// gyj :: com3(rs-485 제어) 기능 추가..	:: send를 먼저 하는 프로세스는  recv감시 thread를 가동하면 안됨.   즉  data를 보낸후에,  수신mode로 전환후에,  수신을 기다려야 한다.
	BOOL		m_testRecv;
#endif
	BOOL        check;
	HANDLE		m_hComm;				// 통신 포트 파일 핸들
	CString		m_sPortName;			// 포트 이름 (COM1 ..)
	BOOL		m_bConnected;			// 포트가 열렸는지 유무를 나타냄.
	OVERLAPPED	m_osRead, m_osWrite;	// 포트 파일 Overlapped structure
	HANDLE		m_hThreadWatchComm;		// Watch함수 Thread 핸들.
	WORD		m_wPortID;				// WM_COMM_READ와 함께 보내는 인수.
	CQueue      m_QueueRead;			//큐버퍼
	//--------- 외부 사용 함수 ------------------------------------//
	void	ClosePort();				//포트 닫기
	DWORD	WriteComm( BYTE *pBuff, DWORD nToWrite);//포트에 데이터 쓰기

	//--------- 내부 사용 함수 ------------------------------------//
	DWORD	ReadComm( BYTE *pBuff, DWORD nToRead);//포트에서 데이터 읽어오기

};

// Thread로 사용할 함수 
DWORD	ThreadWatchComm(CCommThread* pComm);

#endif // !defined(AFX_COMMTHREAD_H__E90CA7C3_844A_433E_88C0_6B6348B0ADDB__INCLUDED_)
