#include <process.h>

bool SignalProcessor::serialPortInit()
{
    if(port.openPort()<0)
        return false;

    port.setPort();

    return true;
}

bool SignalProcessor::recevData(std::string &s)
{
    char *buff = new char[msg_length];
    int recvLen = port.recvData(buff,msg_length);

    if(recvLen>0)
    {
        for(size_t i = 0;i<recvLen;++i)
            s = s+buff[i];

        delete []buff;
        return true;
    }

    delete []buff;
    return false;
}

bool SignalProcessor::sendData(const std::string &s)
{
    int sendLen = port.sendData(s);
    if(sendLen>0)
        return true;

    return false;
}

void SignalProcessor::process()
{
    if(!serialPortInit())
    {
        std::cout<<"serial port open error!"<<std::endl;
        return;
    }

    while(ros::ok())
    {
        ros::spinOnce();

        std::string data;
        if(recevData(data))
        {
            if(data.size()>0)
            {
                std_msgs::String str;
                str.data = data;

                puber.publish(str);
            }
        }
    }

    port.closePort();
}
