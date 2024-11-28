#ifndef __PCHAR_IOCTL_H
#define __PCHAR_IOCTL_H

#include<linux/ioctl.h>

typedef struct devinfo{

	short size;
	short len;
	short avil;
}devinfo_t;

/*#define FIFO_CLEAR  0X01
#define FIFO_START_TIMER 0X02
#define FIFO_GETINFO 0X03
#define FIFREEZE 0X04
#define FIFO_STOP_TIMER 0X05
*/

#define FIFO_CLEAR  _IO('x',1)
#define FIFO_GETINFO  _IOR('x',3,devinfo_t)
#define FIFO_START_TIMER  _IO('x',2)
#define FIFO_STOP_TIMER  _IO('x',4)


#endif
