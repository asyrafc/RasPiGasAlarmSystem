
#include <unistd.h>   //Used for UART
#include <fcntl.h>    //Used for UART
#include <termios.h>  //Used for UART
#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <softTone.h>
#include <pcf8574.h>
#include <lcd.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <my_global.h>
#include <mysql.h>

#define LCDOFS(x) (lcdofs + x)
#define GAS_PIN 19
#define BUZZER_PIN 16

#define	TRUE	            (1==1)
#define	FALSE	            (!TRUE)
#define CHAN_CONFIG_SINGLE  8
#define CHAN_CONFIG_DIFF    0

const int lcdofs = 0x64;
static int myFd ;

void printText(int fd, char message[])
{
    lcdHome(fd);
    lcdClear(fd);
    lcdPrintf(fd, message);
    //delay(500);
}

void printText2(int fd, char message[], float data)
{
    lcdHome(fd);
    lcdClear(fd);
    lcdPrintf(fd, message );
    //delay(500);
}
 
void loadSpiDriver()
{
    if (system("gpio load spi") == -1)
    {
        fprintf (stderr, "Can't load the SPI driver: %s\n", strerror (errno)) ;
        exit (EXIT_FAILURE) ;
    }
}
 
void spiSetup (int spiChannel)
{
    if ((myFd = wiringPiSPISetup (spiChannel, 1000000)) < 0)
    {
        fprintf (stderr, "Can't open the SPI bus: %s\n", strerror (errno));
        exit (EXIT_FAILURE) ;
    }
}
 
int myAnalogRead(int spiChannel,int channelConfig,int analogChannel)
{
    unsigned char buffer[3] = {1}; // start bit
    buffer[1] = (channelConfig+analogChannel) << 4;
    wiringPiSPIDataRW(spiChannel, buffer, 3);
    //printf("Analogue Read : %d - %d - %d \n",buffer[0], buffer[1], buffer[2]);
    return ( (buffer[1] & 3 ) << 8 ) + buffer[2]; // get last 10 bits
}

int main(void)
{
    int fd;
    //char input;
	int loadSpi=FALSE;
    int channelConfig=CHAN_CONFIG_SINGLE;

	if(loadSpi==TRUE)
	{
		loadSpiDriver();
	}
	
    pcf8574Setup(lcdofs, 0x3f);
    wiringPiSetup();
    wiringPiSetupGpio();
	pinMode(GAS_PIN,INPUT);
	pinMode(BUZZER_PIN,OUTPUT);
	softToneCreate(BUZZER_PIN);
    

    pinMode(LCDOFS(1), OUTPUT);
    pinMode(LCDOFS(3), OUTPUT);
    digitalWrite(LCDOFS(1), 0);
    digitalWrite(LCDOFS(3), 1);

	float data = 0.0;
	float data2 = 0.0;
	
    fd = lcdInit(4, 20, 4, LCDOFS(0), LCDOFS(2),LCDOFS(4), LCDOFS(5), LCDOFS(6), LCDOFS(7), 0, 0, 0, 0);

    int uart0_filestream = -1;
    uart0_filestream = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY); //Open in non blocking read/write mode

    if (uart0_filestream == -1)     //ERROR - CAN'T OPEN SERIAL PORT
    {
        printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
    }

    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;         //<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;

    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options); // Transmitting
    //----- TX BYTES -----
    unsigned char tx_buffer[] = "\r";
    unsigned char tx_buffer1[] = "AT+CMGF=1\r\n";
    unsigned char tx_buffer2[] = "AT+CMGS=\"+60175878715\"\r\n";
    unsigned char tx_buffer3[100] = "WARNING!!! Gas Leakage Detected \nPPM is : ";
	unsigned char tx_buffer5[] = "\r\n";
    unsigned char tx_buffer4[]="\032";
	 //---- RX BYTES ----
    //unsigned char rx_buffer[256];

	//printText(fd,"test");
	
	spiSetup(0);
	//float lastPPM;
	
	MYSQL *con = mysql_init(NULL);
	
	if(con == NULL)
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		exit(0);
	}
	
	if(mysql_real_connect(con, "localhost", "root", "12345", "alarmsystem", 0, NULL, 0) == NULL)
	{
		//finish_with_error(con);
	}
	
	char q[1024];
	char q2[1024];
	char timeString[100];
	
	int delayTime = 0;
	
	while(1)
	{	
		time_t now;
		time(&now);

		struct tm* now_tm;
	
		now_tm = localtime(&now);

		char out[80];
		char out2[80];
		char outM[80];
		strftime (out, 80, "%Y-%m-%d|%H:%M:%S", now_tm);
		strftime (out2, 80, "%H", now_tm);
		strftime (outM, 80, "%M", now_tm);
		
		int outInt = atoi(out2);
		int outIntM = atoi(outM);
		//printf("%i", outInt);

		//printf("Data :  %d\n",myAnalogRead(0,channelConfig,7));
		data = myAnalogRead(0,channelConfig,7);
		//data2 = (((log(data/10)-0.21)/-0.47)+2.3);
		data2 = 26.572*exp(1.2894*(data*3.3/3096));
		
		//lastPPM = data2;
		
		char lcdBuffer[100];
		char lcdBuffer2[100];
		char dataBuffer[100];
		
		char dataToString[10];
		//char smsString[1024];
		snprintf(dataToString,10,"%.2f",data2);
		
		//unsigned char smsBuffer[500];
		
		//sprintf(smsString,"WARNING!!! Gas Leakage Detected PPM is : %s \r\n", dataToString);
		
		sprintf(dataBuffer, "%g", data2);
		
		strcpy(lcdBuffer, "Gas Leak            PPM : ");
		strcat(lcdBuffer, dataBuffer);
		
		strcpy(lcdBuffer2, "Safe level          PPM : ");
		strcat(lcdBuffer2, dataBuffer);
		
		//strcat((char*)tx_buffer3,dataToString);
		//strcat(&tx_buffer3,(char*)tx_buffer3a);
		//strcat((char*)smsBuffer,"| PPM : ");
		//strcat((char*)smsBuffer,);
		//strcat((char*)smsBuffer,"\r\n");
		//printf("PPM : %f\n",pow(data2,10));
		
		//int data2Unsigned;
		//data2Unsigned = (unsigned int) data2;
		
		//char buff[1];
		
		//printf("Unsigned Data : %c",data2Unsigned);
		//gcvt(data2,6,buff);
		//
		//unsigned char closing[] = "\r\n";
		memcpy(&tx_buffer3[42],&dataToString, sizeof(char*));
		//memcpy(tx_buffer3,&closing,sizeof(unsigned char));
		
		//strcat((char*)tx_buffer3,dataToString);
		
		if(data2 > 36.0)
        {
			printf("WARNING!!! Gas Leakage Detected \n");
			printf("PPM : %f\n", data2);
			
			sprintf(timeString," %s",out);
			sprintf(q,"Insert into status (ppm,time,status) values ('%s','%s','ada gas')",dataToString,timeString);
			
			if(mysql_query(con, q))
			{
				
			}
			
			printText(fd,lcdBuffer);
			if (uart0_filestream != -1)
			{
				printf("%s\n",tx_buffer);
				write(uart0_filestream,tx_buffer ,sizeof(tx_buffer) );
				usleep(1000000);
			
				printf("%s\n",tx_buffer1);
				write(uart0_filestream,tx_buffer1 ,sizeof(tx_buffer1) );
				usleep(1000000);

				printf("%s\n",tx_buffer2);
				write(uart0_filestream,tx_buffer2 ,sizeof(tx_buffer2) );
				usleep(1000000);

				printf("%s\n",tx_buffer3);
				write(uart0_filestream,tx_buffer3 ,sizeof(tx_buffer3) );
				usleep(1000000);

				printf("%s\n",tx_buffer5);
				write(uart0_filestream,tx_buffer5 ,sizeof(tx_buffer5) );
				usleep(1000000);
				
				printf("%s\n",tx_buffer4);
				write(uart0_filestream,tx_buffer4 ,sizeof(tx_buffer4) );
				usleep(1000000);
			}
			
			int i;
			for(i=0;i<3;i++)
			{
				softToneWrite(BUZZER_PIN, 1);
				delay(1000);
				softToneWrite(BUZZER_PIN, 2);
				delay(1000);
				softToneWrite(BUZZER_PIN, 3);
				delay(1000);
			}
			softToneWrite(BUZZER_PIN, 0);
			//delay(10000);
        }
        else
        {
            printf("Safe level of gas \n");	    
			printText(fd,lcdBuffer2);
			softToneWrite(BUZZER_PIN, 0);
			
			sprintf(timeString," %s",out);
			sprintf(q,"Insert into status (ppm,time,status) values ('%s','%s','x ada gas')",dataToString,timeString);
			
			if(mysql_query(con, q))
			{
				
			}
        }
		
		if(outInt >= 11 && outInt <= 14)
        {
            delayTime = 5000;
        }
		else if(outInt == 00)
		{
			if(outIntM >= 0 && outIntM <= 0)
			{
				sprintf(q2,"truncate status");
				if(mysql_query(con, q2)){}
			}
			delayTime = 10000;
		}
		else 
		{
			delayTime = 10000;
		}
		
        delay(delayTime);
    }
	close (myFd);
	mysql_close(con);
    return 0;
}
