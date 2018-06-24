#ifndef PROCESS_H_
#define PROCESS_H_

#include <ros/ros.h>
#include <std_msgs/String.h>

#include <serial_port.h>
#include <string>
#include <iostream>

class SignalProcessor
{
public:
    SignalProcessor()
    {
        int baudrate;
        int tty;
        std::string tty_dev;

        ros::param::get("~baudrate",baudrate);
        ros::param::get("~tty",tty);
        ros::param::get("~tty_dev",tty_dev);
        ros::param::get("~msg_length",msg_length);

        port = SerialPort(baudrate,tty,tty_dev);

        puber = nh.advertise<std_msgs::String>("msgs_in",1);
        suber = nh.subscribe("msgs_out",1,&SignalProcessor::msgsOutCB,this);
    }

    void process();
private:
    SerialPort port;

    ros::NodeHandle nh;

    ros::Publisher puber;
    ros::Subscriber suber;

    int msg_length = 20;

    bool serialPortInit();
    bool recevData(std::string &);
    bool sendData(const std::string &);

    void msgsOutCB(const std_msgs::String &msg)
    {
        if(!sendData(msg.data))
            std::cout<<"msg send failed."<<std::endl;
    }
};

#endif
