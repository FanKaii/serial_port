#include <serial_port.h>

int SerialPort::setPort()
{
    int fdcom_ = fdcom;
    struct termios termios_old, termios_new;
    int     baudrate, tmp;
    char    databit, stopbit, parity, fctl;

    bzero(&termios_old, sizeof(termios_old));
    bzero(&termios_new, sizeof(termios_new));
    cfmakeraw(&termios_new);
    tcgetattr(fdcom_, &termios_old);         //get the serial port attributions
    /*------------设置端口属性----------------*/
    //baudrates
    baudrate = convbaud();
    cfsetispeed(&termios_new, baudrate);        //填入串口输入端的波特率
    cfsetospeed(&termios_new, baudrate);        //填入串口输出端的波特率
    termios_new.c_cflag |= CLOCAL;          //控制模式，保证程序不会成为端口的占有者
    termios_new.c_cflag |= CREAD;           //控制模式，使能端口读取输入的数据

    // 控制模式，flow control
    fctl = port.fctl;
    switch(fctl){
        case '0':{
            termios_new.c_cflag &= ~CRTSCTS;        //no flow control
        }break;
        case '1':{
            termios_new.c_cflag |= CRTSCTS;         //hardware flow control
        }break;
        case '2':{
            termios_new.c_iflag |= IXON | IXOFF |IXANY; //software flow control
        }break;
    }

    //控制模式，data bits
    termios_new.c_cflag &= ~CSIZE;      //控制模式，屏蔽字符大小位
    databit = port.databit;
    switch(databit){
        case '5':
            termios_new.c_cflag |= CS5;
        case '6':
            termios_new.c_cflag |= CS6;
        case '7':
            termios_new.c_cflag |= CS7;
        default:
            termios_new.c_cflag |= CS8;
    }

    //控制模式 parity check
    parity = port.parity;
    switch(parity){
        case '0':{
            termios_new.c_cflag &= ~PARENB;     //no parity check
        }break;
        case '1':{
            termios_new.c_cflag |= PARENB;      //odd check
            termios_new.c_cflag &= ~PARODD;
        }break;
        case '2':{
            termios_new.c_cflag |= PARENB;      //even check
            termios_new.c_cflag |= PARODD;
        }break;
    }

    //控制模式，stop bits
    stopbit = port.stopbit;
    if(stopbit == '2'){
        termios_new.c_cflag |= CSTOPB;  //2 stop bits
    }
    else{
        termios_new.c_cflag &= ~CSTOPB; //1 stop bits
    }

    //other attributions default
    termios_new.c_oflag &= ~OPOST;          //输出模式，原始数据输出
    termios_new.c_cc[VMIN]  = 1;            //控制字符, 所要读取字符的最小数量
    termios_new.c_cc[VTIME] = 1;            //控制字符, 读取第一个字符的等待时间    unit: (1/10)second

    tcflush(fdcom_, TCIFLUSH);               //溢出的数据可以接收，但不读
    tmp = tcsetattr(fdcom_, TCSANOW, &termios_new);  //设置新属性，TCSANOW：所有改变立即生效    tcgetattr(fdcom, &termios_old);
    return(tmp);
}

int SerialPort::openPort()
{
    std::string ptty;

    ptty = get_ptty();

    fdcom = open(ptty.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);

    return fdcom; 
}

void SerialPort::closePort()
{
    close(fdcom);
}

int SerialPort::sendData(const std::string &data)
{
    int len = 0;
    int datalen = data.size();
    len = write(fdcom, data.c_str(), datalen);  //实际写入的长度
    if(len == datalen)
        return (len);
    else
    {
        tcflush(fdcom, TCOFLUSH);
        return -1;
    }
}

int SerialPort::recvData(char *data,size_t dataLength)
{
    int datalen = dataLength;
    int readlen, fs_sel;
    fd_set  fs_read;
    struct timeval tv_timeout;

    FD_ZERO(&fs_read);
    FD_SET(fdcom, &fs_read);

    fs_sel = select(fdcom+1, &fs_read, NULL, NULL, &tv_timeout);
    if(fs_sel)
    {
        readlen = read(fdcom, data, datalen);
        return(readlen);
    }
    else
        return -1;

    return readlen;
}

