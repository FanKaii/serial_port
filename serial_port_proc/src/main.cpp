#include <process.h>

int main(int argc,char **argv)
{
    ros::init(argc,argv,"serial_port_node");

    SignalProcessor sp;

    sp.process();

    return 0;
}
