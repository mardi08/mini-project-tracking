#include "serialib.h"
#include <iostream>
#include <stdio.h>

#if defined (_WIN32) || defined( _WIN64)
#define         DEVICE_PORT             "COM1"                               // COM1 for windows
#endif

#ifdef __linux__
#define         DEVICE_PORT             "/dev/ttyUSB0"                         // ttyS0 for linux
#endif

using namespace std;

int main () {
  for(int i = 0; i < 1000000; i++);
  
  while(1) {
    serialib LS;
    int Ret;

    // open serial port
    Ret = LS.Open(DEVICE_PORT, 115200);

    if (Ret != 1) {
      printf("Error opening port!");
    } else {
      printf("Port opened successfully!");

      Ret = LS.WriteString("test\n");
      if (Ret!=1) {                                                           // If the writting operation failed ...
        printf ("Error while writing data\n");                              // ... display a message ...                                                        // ... quit the application.
      } else {
        printf ("Write operation is successful \n");
      }
    }
    
    LS.Close();
  }
  return(0);
}

  
