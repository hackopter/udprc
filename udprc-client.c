#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

#define ADDR       "10.2.110.201"
#define PORT       25000
#define BUF_SIZ    4

#define JOY_DEV "/dev/input/js0" /* joystick device path */

int main (int argc, char **argv) {

    int joy_fd, *axis=NULL, num_of_axis=0, num_of_buttons=0, x;
    char *button=NULL, name_of_joystick[80];

/***********************  to be used if mapping is required  **************************

    double slope = (255.0 / 65535.0);
    int output_end = 255, output_start = 0, input_end = 32768, input_start = -32768;

***************************************************************************************/


	struct js_event js;

if( ( joy_fd = open( JOY_DEV , O_RDONLY)) == -1 ) 
	{
		printf( "Couldn't open joystick\n" );
		return -1;
	}

ioctl( joy_fd, JSIOCGAXES, &num_of_axis );  
	ioctl( joy_fd, JSIOCGBUTTONS, &num_of_buttons );
	ioctl( joy_fd, JSIOCGNAME(80), &name_of_joystick );

	axis = (int *) calloc( num_of_axis, sizeof( int ) ); 
							     

	button = (char *) calloc( num_of_buttons, sizeof( char ) ); //same as above

	printf("Joystick detected: %s\n\t%d axis\n\t%d buttons\n\n"
		, name_of_joystick
		, num_of_axis
		, num_of_buttons );

    int sock;
    struct sockaddr_in saddr, caddr;
   uint8_t i;
   int16_t buf[BUF_SIZ];
   //double buf[BUF_SIZ];

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0) {
	perror("ERROR:");
	exit (-1);
    }

    caddr.sin_family = AF_INET;
    caddr.sin_addr.s_addr = htonl(INADDR_ANY);
    caddr.sin_port = htons(0); //port apto opoio tha fevgoun data apton client = indifferent

//server data also needed in client code

    saddr.sin_family = AF_INET;
    inet_pton(AF_INET, ADDR, &(saddr.sin_addr));
    saddr.sin_port = htons(PORT);

    if (bind(sock,(struct sockaddr *) &caddr, sizeof(caddr)) < 0) {
	perror("ERROR:");
	exit (-1);
    }

//fcntl( joy_fd, F_SETFL, O_NONBLOCK ); // change accordingly

while( 1 ) 	/* infinite loop */
	{

			/* read the joystick state */
		read(joy_fd, &js, sizeof(struct js_event)); //attempts to read up to sizeof(...) bytes from joy_fd, into the buffer starting at js
							     
		
			/* see what to do with the event */
		switch (js.type & ~JS_EVENT_INIT)
		{
			case JS_EVENT_AXIS:
				axis   [ js.number ] = js.value; //'analogue' value to be processed/sent
				break;
			case JS_EVENT_BUTTON:
				button [ js.number ] = js.value; // 0 or 1
				break;
		}


    //for (i = 0; i<=255; i++) {
	memset(buf, 0, sizeof(buf));

	buf[0] = axis[0];   //  ABS_X
	buf[1] = axis[1];   //  ABS_Y
	buf[2] = axis[3];   //  ABS_Z
	buf[3] = axis[4];   //  ABS_RX

	//buf[0] = output_start + slope * (axis[0] - input_start);	
	//buf[1] = output_start + slope * (axis[1] - input_start);
	//buf[2] = output_start + slope * (axis[2] - input_start);
        //buf[4] = axis[4];   //  ABS_RZ
        //buf[5] = axis[5];
        //buf[6] = axis[6];


	if(sendto(sock,
		  &buf,
		  sizeof(buf),
		  0,
		  (struct sockaddr *) &saddr, //vazw ta dedomena tou server ston opoio sindeomai
		  sizeof saddr) < 0) { //den to pernaw me diefthinsi
	    perror("ERROR:");
	    exit (-1);
}
	
	printf("%d %d %d %d\n", buf[0], buf[1], buf[2], buf[3]);
	//printf("%.f %.f %.f\n", buf[0], buf[1], buf[2]);
	
    }

    return 1;
}
