#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "rs232.h"

int main()
{
  int i=0,
      cport_nr=0,        /* /dev/ttyS0 (COM1 on windows) */
      bdrate=115200, n;       /* 9600 baud */
      
  char mode[]={'8','N','1',0},
       str[2][512];
       
  unsigned char buf[4096];
       
  strcpy(str[0], "The quick brown fox jumped over the lazy grey dog.\n");
  strcpy(str[1], "Happy serial programming!\n");
  
  if(RS232_OpenComport(cport_nr, bdrate, mode, 0))
  {
    printf("Can not open comport\n");

    return(0);
  } else{
    printf("opened comport\n");
  }
  
  while(1)
  {
    //RS232_cputs(cport_nr, str[i]);

    //printf("sent: %s\n", str[i]);
    
    n = RS232_PollComport(cport_nr, buf, 4095);

    if(n > 0)
    {
//      buf[n] = 0;   /* always put a "null" at the end of a string! */
//
//      for(i=0; i < n; i++)
//      {
//        if(buf[i] < 32)  /* replace unreadable control-codes by dots */
//        {
//          buf[i] = '.';
//        }
//      }

      printf("received %i bytes: %s\n", n, (char *)buf);
    }

#ifdef _WIN32
    Sleep(1000);
#else
    usleep(100000);  /* sleep for 1 Second */
#endif

    i++;

    i %= 2;
  }

  return(0);
}
