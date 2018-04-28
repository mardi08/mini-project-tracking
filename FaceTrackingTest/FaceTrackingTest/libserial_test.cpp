#include <SerialStream.h>
#include <iostream>
#include <unistd.h>
//#include <cstdlib>
#include <string.h>

//place this in main, before the while loop
int main() {
    char next_char[100] = "";
    int i;

    using namespace std;
    using namespace LibSerial ;
    SerialStream my_serial_stream;
    my_serial_stream.Open("/dev/ttyUSB0") ;
    my_serial_stream.SetBaudRate( SerialStreamBuf::BAUD_115200 ) ;
    my_serial_stream.SetCharSize( SerialStreamBuf::CHAR_SIZE_8 ) ;
    my_serial_stream.SetFlowControl( SerialStreamBuf::FLOW_CONTROL_NONE ) ;
    my_serial_stream.SetParity( SerialStreamBuf::PARITY_NONE ) ;
    my_serial_stream.SetNumOfStopBits(1) ;
    my_serial_stream.SetVTime(1);
    my_serial_stream.SetVMin(100);

    cout<<"Sending Command:\n";
    my_serial_stream << "Hello, Serial Port_1." << std::endl ;

    my_serial_stream.read(next_char,100);
    cout<<"Result: "<<next_char<<"\n";

    my_serial_stream.Close();

    my_serial_stream.Open("/dev/ttyUSB0") ;
    my_serial_stream.SetBaudRate( SerialStreamBuf::BAUD_115200 ) ;
    my_serial_stream.SetCharSize( SerialStreamBuf::CHAR_SIZE_8 ) ;
    my_serial_stream.SetFlowControl( SerialStreamBuf::FLOW_CONTROL_NONE ) ;
    my_serial_stream.SetParity( SerialStreamBuf::PARITY_NONE ) ;
    my_serial_stream.SetNumOfStopBits(1) ;
    my_serial_stream.SetVTime(1);
    my_serial_stream.SetVMin(100);

    cout<<"Sending Command:\n";
    my_serial_stream << "Hello, Serial Port_2." << std::endl ;

    my_serial_stream.read(next_char,100);
    cout<<"Result: "<<next_char<<"\n";

    my_serial_stream.Close();
    return EXIT_SUCCESS;
}
