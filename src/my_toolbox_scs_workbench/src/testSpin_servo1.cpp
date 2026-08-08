#include <iostream>
#include "SCServo.h"
////////////////////////////////////////////////////////////
SCSCL scs_wb;

u8 ID[1] = {11};
u16 Position[1];
u16 Speed[1];
////////////////////////////////////////////////////////////

int main()
{
    const char* port_name = "/dev/ttyACM0";
    int         baud_rate = 1000000;

    if(!scs_wb.begin(baud_rate, port_name)){
        std::cout<<"Failed to init scscl motor!"<<std::endl;
        return 0;
    }
	while(1){
		Position[0] = 1000;
		Speed[0] = 1500;                // unit of 0.0146rpm
		scs_wb.SyncWritePos(ID, 1, Position, 0, Speed);
		std::cout<<"pos = "<<1000<<std::endl;
		usleep(754*1000);//[(P1-P0)/V]*1000+100
  
		Position[0] = 20;
		Speed[0] = 1500;
		scs_wb.SyncWritePos(ID, 1, Position, 0, Speed);
		std::cout<<"pos = "<<20<<std::endl;
		usleep(754*1000);//[(P1-P0)/V]*1000+100
	}
	scs_wb.end();
	return 1;
}
////////////////////////////////////////////////////////////



