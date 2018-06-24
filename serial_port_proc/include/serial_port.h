#ifndef SERIAL_PORT_H_
#define SERIAL_PORT_H_

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <string>

class SerialPort
{
public:
    SerialPort() = default;
    SerialPort(int baudrate,int tty,const std::string &def_tty_dev):baudrate(baudrate),tty(tty),tty_dev(def_tty_dev)
    {
        port = {
                '0',                            // print prompt after receiving
                this->baudrate,                 // baudrate
                '8',                            // databit: 8
                '0',                            // debug: off
                '0',                            // echo: off
                '2',                            // flow control: software
                this->tty,                      // tty
                '0',                            // parity: none
                '1',                            // stopbit: 1
                 0                              // reserved
            };
    }

    /*
     * 打开串口，返回文件描述符
    */
    int openPort();

    /*
     * 设置串口
    */
    int setPort();

    /*
     * 关闭串口
    */
    void closePort();

    /*
     * 发送数据
     * 返回实际发送长度
    */
    int sendData(const std::string &data);

    /*
     * 接收数据
     * 返回实际读入的长度
    */
    int recvData(char *data,size_t dataLength);
private:
    class PortInfo
    {
    public:
        char prompt;        //prompt after reciving data
        int  baudrate;      //baudrate
        char databit;       //data bits, 5, 6, 7, 8
        char  debug;        //debug mode, 0: none, 1: debug
        char  echo;         //echo mode, 0: none, 1: echo
        char fctl;          //flow control, 0: none, 1: hardware, 2: software
        int  tty;          //tty: 0, 1, 2, 3, 4, 5, 6, 7
        char parity;        //parity 0: none, 1: odd, 2: even
        char stopbit;       //stop bits, 1, 2
        int reserved;       //reserved, must be zero
    };
    PortInfo port;

    int tty;
    int baudrate;

    std::string tty_dev;
    std::string port_name;

    int fdcom = 0;

    /*
     * 获得端口名称
    */
    const std::string &get_ptty();

    /*
     * 波特率转换函数
    */
    int convbaud();
};

inline const std::string &SerialPort::get_ptty()
{
    port_name = tty_dev+std::to_string(tty);
    return port_name;
}

inline int SerialPort::convbaud()
{
    switch(baudrate)
    {
    case 2400:
        return B2400;
    case 4800:
        return B4800;
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    default:
        return B9600;
    }
}

#endif

