/*===============================================================================
* Program to parse NMEA format string. http://www.catb.org/gpsd/NMEA.html#_gst_gps_pseudorange_noise_statistics
* This program accepts input strings only in following formats:
* 1. GPGGA
* 2. GPGSV
* 3. GPGSA
* 4. GPGST
* 5. GPGLL
* 
*
* NMEA Sentence formats:
*
* $GPGGA,hhmmss,x.x,N,x.x,E,x,x,x.x,x.x,M,x.x,M,x,x,*cs
*
* $GPGSV,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,*cs
*
* $GPGSA,A/M,x,x,x,x,x,x,x,x,x,x,x,x,x,x.x,x.x,x.x,*cs
*
* $GPGST,hhmmss,x.x,x.x,x.x,x.x,x.x,x.x,x.x,*cs
*
* $GPGLL,x.x,N,x.x,W,hhmmss,A/V,*cs
* where x is a number, hh - hours, mm - minutes, ss - seconds and cs - checksum(in hex)
*
* Examples:
*
* $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,,*47
*
* $GPGSV,2,1,08,01,40,083,46,02,17,308,41,12,07,344,39,14,22,228,45,*75
*
* $GPGSA,A,3,04,05,,09,12,,,24,,,,,2.5,1.3,2.1,*39
*
* $GPGST,172814,0.006,0.023,0.020,273.6,0.023,0.020,0.031,*6A
*
* $GPGLL,4916.45,N,12311.12,W,225444,A,*34
*
* 
*
* File: NMEAparser.c
* Compilation: gcc NMEAparser.c
* Input is read from a file "message.txt"
*
*
===============================================================================*/


#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

#define MAX_INPUT_LINE_LENGTH 200

/**
 * checkdigit
 * <p>
 * This function is used to check whether the input contains only number values.
 * <p> 
 * 
 * @param  input Buffer containing the input string
 * @param  x specifies whether the input is a float or integer value.
 * @return value -1 if input is not a number else 0.
 */

int checkdigit(char* input, int x) {
	int i=0;
	if(x==0){
		while(input[i]!='\0'){	
			if(!isdigit(input[i])){
				return -1;	
			}
			i++;
		}
	}
	else {
		int countDot=0;
		while(input[i]!='\0'){	
			if(!isdigit(input[i])){
				if((input[i] == '.') && (countDot==0)){
					countDot++;	
				}
				else
					return -1;	
			}
			i++;
		}
	}
	return 0;
}


/**
 *  GPGGAparser                          
 * <p>
 * This function is used for parsing GPGGA format sentences.
 * <p> 
 * 
 * @param  buf Buffer containing the input string read from the file
 * @param  bufsize Size of the input Buffer
 */

void GPGGAParser(char *buf, unsigned int bufSize)
{
	char auxBuf[20];
	char* p2;
	char* p3;
	unsigned int index=7;
	printf("\n\n**********Parsing GPGGA input string**********\n\n");
	// GGA
	buf+=7;
	// Time
	if(index>=bufSize)
	{
		printf("End of string");
		
		return;
	}
	if((p2=strchr(buf,','))==NULL) {
		
		printf("Wrong format\n");
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Fix time not specified\n");
			buf++;
			index++;
		}
		else if (p2-buf!=6) {
			printf("Incorrect time format\n");
			return;
		}
		else {
			
			index+=(p2-buf+1);
			printf("Fix taken at ");
			strncpy(auxBuf, buf, 2);
			auxBuf[2] = '\0';
			int hour = atoi(auxBuf);
			if (hour>23) {
				printf("Wrong value of time. Hour cannot be greater than 23");
				return;
			}
			buf += 2;
			printf("%s:",auxBuf);
			strncpy(auxBuf, buf, 2);
			auxBuf[2] = '\0';
			buf += 2;
			int min = atoi(auxBuf);
			if (min>59) {
				printf("Wrong value of time. Minutes cannot be greater than 59");
				
				return;
			}
			printf("%s:",auxBuf);
			strncpy(auxBuf, buf, 2);
			auxBuf[2] = '\0';
			buf = p2 + 1;
			int sec = atoi(auxBuf);
			if (sec>59) {
				printf("Wrong value of time. Seconds cannot be greater than 59");
				
				return;
			}
			printf("%s UTC\n",auxBuf);
		}
	}
	
	// Latitude
	if(index>=bufSize)
	{
		printf("End of string");
		
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Latitude not specified\n");
			buf++;
			index++;
			p3=strchr(buf,',');	
			if(p3-buf==0) {
				buf++;
				index++;
			}
			else {
				buf=buf+3;
				index+=3;
			}
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if((p2 = strchr(auxBuf, '.')) == NULL) {
				printf("Incorrect format\n");
				return;
			}
			if(p2-auxBuf < 2) {
				return;
			}
			int latitude = atoi(auxBuf);
			if (latitude/100>90 || latitude/100<0) {
				printf("Wrong value of latitude. Latitude measurements range from 0B0 to 90B0.");
				return;
			}
			int min = latitude%100;
			if (min>59) {
				printf("Wrong value of latitude. Minutes cannot be greater than 59");
				return;
			}
			printf("latitude: %d deg %d%s' ", latitude/100,min,p2);
			if((p2 = strchr(buf, ',')) == NULL)
				printf("Wrong format");
			if(p2 - buf != 1) {
				printf("\nNot valid GPGGA sentence format, direction missing\n");
				return;
			}
			if(*buf == 'S')
				printf("S\n");
			else if(*buf == 'N')
				printf("N\n");
			else {
				printf("Wrong direction. Possible values 'N' or 'S'");
				return;
			}
			index+=(p2-buf+1);
			buf = p2 + 1;
		}
	}
	
	// Longitude
	if(index>=bufSize)
	{
		printf("End of string");
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Latitude not specified\n");
			buf++;
			index++;
			p3=strchr(buf,',');	
			if(p3-buf==0) {
				buf++;
				index++;
			}
			else {
				buf=buf+3;
				index+=3;
			}
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if((p2 = strchr(auxBuf, '.')) == NULL) {
				printf("Incorrect format\n");
				return;
			}
			if(p2-auxBuf < 2)
				return;
			int longitude = atoi(auxBuf);
			if (longitude/100>180 || longitude/100<0) {
				printf("Wrong value of longitude. Longitude measurements range from 0B0 to 180B0.");
				return;
			}
			int min = longitude%100;
			if (min>59) {
				printf("Wrong value of longitude. Minutes cannot be greater than 59");
				return;
			}
			printf("longitude: %d deg %d%s' ", longitude/100,min%100,p2);
			if((p2 = strchr(buf, ',')) == NULL)
				printf("Wrong format");
			if(p2 - buf != 1) {
				printf("\nNot valid GPGGA sentence format, direction missing\n");
				return;
			}
			if(*buf == 'E')
				printf("E\n");
			else if(*buf == 'W')
				printf("W\n");
			else {
				printf("Wrong direction. Possible values 'E' or 'W'");
				return;
			}
			index+=(p2-buf+1);
			buf = p2 + 1;
		}
	}
	
	// GPS quality
	if(index>=bufSize)
	{
		printf("End of string");
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Fix Quality not specified\n");
			buf++;
			index++;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,0)<0) {
				printf("Fix Quality must be a number");
				return;
			}
			int q;
			q=atoi(auxBuf);
			if (q==0)
				printf("Quality of fix: 'Invalid Fix'\n");
			else if (q == 1)
				printf("Quality of fix: 'GPS Fix'\n");
			else if (q==2)
				printf("Quality of fix: 'DGPS Fix'\n");
			else {
				printf("Wrong value of Quality of fix specified'\n");
				return;
			}

		}
	}
	
	// Satellites in use
	if(index>=bufSize)
	{
		printf("End of string");
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Number of Satellites not specified\n");
			buf++;
			index++;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,0)<0) {
				printf("Number of Satellites must be a number");
				return;
			}
			int q=atoi(auxBuf);
			if (q<0)
			{
				printf("Number of satellites being tracked canot be negative'\n");
				return;
			}
			if (q>12)
			{
				printf("Number of satellites being tracked canot be greater than 12'\n");
				return;
			}
			printf("Number of satellites being tracked: %d\n", q);
		}
	}
	
	// HDOP
	if(index>=bufSize)
	{
		printf("End of string");
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		return;
	}
	else {
		if(p2-buf==0) {
			printf("HDOP not specified\n");
			buf++;
			index++;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,1)<0) {
				printf("HDOP must be a number");
				return;
			}
			printf("Horizontal dilution of position (HDOP): %s\n", auxBuf);
		}
	}
	
	// Altitude
	if(index>=bufSize)
	{
		printf("End of string");
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Altitude not specified\n");
			buf++;
			index++;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,1)<0) {
				printf("Altitude must be a number");
				return;
			}
			if((p2 = strchr(buf, ',')) == NULL) {
				printf("Wrong format");
				exit(0);
			}
			if(p2 - buf != 1) {
				printf("\nNot valid GPGGA sentence format, Unit of altitude measurement missing\n");
				return;
			}
			if(*buf == 'M')
				printf("Altitude (m) above mean sea level: %s\n", auxBuf);
			else {
				printf("Wrong unit for altitude measurement specified.");
				return;
			}
			index+=(p2-buf+1);
			buf = p2 + 1;
		}
	}
	
	// Height of geoid
	if(index>=bufSize)
	{
		printf("End of string");
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Height of geoid not specified\n");
			buf++;
			index++;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,1)<0) {
				printf("Height of geoid must be a number");
				return;
			}
			if((p2 = strchr(buf, ',')) == NULL) {
				printf("Wrong format");
				exit(0);
			}
			if(p2 - buf != 1) {
				printf("\nNot valid GPGGA sentence format, Unit of geoid height measurement missing\n");
				return;
			}
			if(*buf == 'M')
				printf("Height of geoid (m) above WGS84 ellipsoid: %s\n", auxBuf);
			else {
				printf("Wrong unit for geoid height measurement specified.");
				return;
			}
			index+=(p2-buf+1);
			buf = p2 + 1;
		}
	}
	
	// Time since last DGPS update
	if(index>=bufSize)
	{
		printf("End of string");
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Time since last DGPS update not specified\n");
			buf++;
			index++;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,0)<0) {
				printf("Time must be a number");
				return;
			}
			printf("Time since last DGPS update: %s\n", auxBuf);
		}
	}
	
	// Differential reference station ID
	if(index>=bufSize)
	{
		printf("End of string");
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Differential reference station ID  not specified\n");
			buf++;
			index++;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,0)<0) {
				printf("Differential reference station ID must be a number");
				return;
			}
			printf("Differential reference station ID : %s\n", auxBuf);
		}
	}
	
	// Checksum
	if(buf[0]!='*')
		printf("Not valid GPGGA sentence format. Terminal '*' missing");
	else
		printf("\n**********GPGGA format string parsing complete**********\n");

}

/**
 *  GPGSAparser                          
 * <p>
 * This function is used for parsing GPGSA format sentences.
 * <p> 
 * 
 * @param  buf Buffer containing the input string read from the file
 * @param  bufsize Size of the input Buffer
 */
void GPGSAParser(char *buf, unsigned int bufSize) {
	char auxBuf[20];
	char* p2;
	unsigned int index=7;
	int i;
	printf("\n\n**********Parsing GPGSA input string**********\n\n");
	// GSV
	buf+=7;
	
	// Mode Selection
	if(index>=bufSize)
	{
		printf("End of string");
		
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		
		printf("Wrong format\n");
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Mode Selection not specified\n");
			buf++;
			index++;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (strncmp (auxBuf,"A",1)==0)
				printf("Mode: Auto selection\n");
			else if (strncmp (auxBuf,"M",1)==0)
				printf("Mode: Manual Selection\n");
			else {
				printf("Invalid Mode\n");
				return;
			}
		}
	}

	// Fix type
	if(index>=bufSize)
	{
		printf("End of string");
		
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Fix type not specified\n");
			buf++;
			index++;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,0)<0) {
				printf("Fix Type must be a number");
				return;
			}
			int q;
			q=atoi(auxBuf);
			if (q==1)
				printf("Fix Type: 'No fix'\n");
			else if (q == 2)
				printf("Fix Type: '2D fix'\n");
			else if (q==3)
				printf("Fix Type: '3D fix'\n");
			else {
				printf("Invalid Fix Type\n");
				
				return;
			}
		}
	}

	// PRNs of Satellites
	for(i=0;i<12;i++) {
		if(index>=bufSize)
		{
			printf("End of string");
			return;
		}
		if((p2=strchr(buf,','))==NULL) {	
			printf("Wrong format\n");
			return;
		}
		else {
			if(p2-buf==0) {
				printf("PRN of Satellite # %d : not specified\n",i);
				buf++;
				index++;
			}
			else {
				index+=(p2-buf+1);
				strncpy(auxBuf, buf, p2 - buf);
				auxBuf[p2 - buf] = '\0';
				buf = p2 + 1;
				if (checkdigit(auxBuf,0)<0) {
					printf("PRN of Satellite must be a number");
					return;
				}
				printf("PRN of Satellite # %d : %s\n",i,auxBuf);
			}
		}
	}
	
	// PDOP
	if(index>=bufSize)
	{
		printf("End of string");
		
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		
		return;
	}
	else {
		if(p2-buf==0) {
			printf("PDOP not specified\n");
			buf++;
			index++;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,1)<0) {
				printf("PDOP must be a number");
				return;
			}
			printf("PDOP: %s\n", auxBuf);
		}
	}

	// HDOP
	if(index>=bufSize)
	{
		printf("End of string");
		
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		
		return;
	}
	else {
		if(p2-buf==0) {
			printf("HDOP not specified\n");
			buf++;
			index++;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,1)<0) {
				printf("HDOP must be a number");
				return;
			}
			printf("HDOP: %s\n", auxBuf);
		}
	}

	// VDOP
	if(index>=bufSize)
	{
		printf("End of string");
		
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		
		return;
	}
	else {
		if(p2-buf==0) {
			printf("VDOP not specified\n");
			buf++;
			index++;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,1)<0) {
				printf("VDOP must be a number");
				return;
			}
			printf("VDOP: %s\n", auxBuf);
		}
	}
	// Checksum
	if(buf[0]!='*')
		printf("Not valid GPGSA sentence format. Terminal '*' missing");
	else
		printf("\n**********GPGSA format string parsing complete**********\n");
	
}

/**
 *  GPGLLparser                          
 * <p>
 * This function is used for parsing GPGLL format sentences.
 * <p> 
 * 
 * @param  buf Buffer containing the input string read from the file
 * @param  bufsize Size of the input Buffer
 */

void GPGLLParser(char *buf, unsigned int bufSize)
{
	char auxBuf[20];
	char* p2;
	char* p3;
	unsigned int index=7;

	printf("\n**********Parsing GPGLL input string**********\n\n");
	// GGA
	buf+=7;
	
	// Latitude
	if(index>=bufSize)
	{
		
		printf("End of string");
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		
		printf("Wrong format\n");
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Latitude not specified\n");
			buf++;
			index++;
			p3=strchr(buf,',');	
			if(p3-buf==0) {
				buf++;
				index++;
			}
			else {
				buf=buf+3;
				index+=3;
			}
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,1)<0) {
				printf("Latitude must be a number");
				return;
			}
			if((p2 = strchr(auxBuf, '.')) == NULL) {
				return;
			}
			if(p2-auxBuf < 2){
				return;
			}
			int latitude = atoi(auxBuf);
			if (latitude/100>90 || latitude/100<0) {
				printf("Wrong value of latitude. Latitude measurements range from 0B0 to 90B0.");
				return;
			}
			int min = latitude%100;
			if (min>59) {
				printf("Wrong value of latitude. Minutes cannot be greater than 59");
				return;
			}
			printf("latitude: %d deg %d%s' ", latitude/100,min,p2);
			if((p2 = strchr(buf, ',')) == NULL)
				printf("Wrong format");
			if(p2 - buf != 1) {
				printf("\nNot valid GPGGA sentence format, direction missing\n");
				return;
			}
			if(*buf == 'S')
				printf("S\n");
			else if(*buf == 'N')
				printf("N\n");
			else {
				printf("Wrong direction. Possible values 'N' or 'S'");
				return;
			}
			index+=(p2-buf+1);
			buf = p2 + 1;
		}
	}
	
	// Longitude
	if(index>=bufSize)
	{
		printf("End of string");
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Latitude not specified\n");
			buf++;
			index++;
			p3=strchr(buf,',');	
			if(p3-buf==0) {
				buf++;
				index++;
			}
			else {
				buf=buf+3;
				index+=3;
			}
		}
		else {
			if(buf== NULL) return;
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,1)<0) {
				printf("Longitude must be a number");
				return;
			}
			if((p2 = strchr(auxBuf, '.')) == NULL)
				return;
			if(p2-auxBuf < 2)
				return;
			int longitude = atoi(auxBuf);
			if (longitude/100>180 || longitude/100<0) {
				printf("Wrong value of longitude. Longitude measurements range from 0B0 to 180B0.");
				return;
			}
			int min = longitude%100;
			if (min>59) {
				printf("Wrong value of longitude. Minutes cannot be greater than 59");
				return;
			}
			printf("longitude: %d deg %d%s' ", longitude/100,min%100,p2);
			if((p2 = strchr(buf, ',')) == NULL)
				printf("Wrong format");
			if(p2 - buf != 1) {
				printf("\nNot valid GPGGA sentence format, direction missing\n");
				return;
			}
			if(*buf == 'E')
				printf("E\n");
			else if(*buf == 'W')
				printf("W\n");
			else {
				printf("Wrong direction. Possible values 'E' or 'W'");
				return;
			}
			index+=(p2-buf+1);
			buf = p2 + 1;
		}
	}

	// Time
	if(index>=bufSize)
	{
		printf("End of string");
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Fix time not specified\n");
			buf++;
			index++;
		}
		else if (p2-buf!=6) {
			printf("Incorrect time format\n");
			return;
		}
		else {
			index+=(p2-buf+1);
			printf("Fix taken at ");
			strncpy(auxBuf, buf, 2);
			auxBuf[2] = '\0';
			if (checkdigit(auxBuf,0)<0) {
				printf("Time must be a number");
				return;
			}
			int hour = atoi(auxBuf);
			if (hour>23) {
				printf("Wrong value of time. Hour cannot be greater than 23");
				return;
			}
			buf += 2;
			printf("%s:",auxBuf);
			strncpy(auxBuf, buf, 2);
			auxBuf[2] = '\0';
			buf += 2;
			int min = atoi(auxBuf);
			if (min>59) {
				printf("Wrong value of time. Minutes cannot be greater than 59");
				return;
			}
			printf("%s:",auxBuf);
			strncpy(auxBuf, buf, 2);
			auxBuf[2] = '\0';
			buf = p2 + 1;
			int sec = atoi(auxBuf);
			if (sec>59) {
				printf("Wrong value of time. Seconds cannot be greater than 59");
				return;
			}
			printf("%s UTC\n",auxBuf);
		}
	}

	// Status
	if(index>=bufSize)
	{
		printf("End of string");
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Status not specified\n");
			buf++;
			index++;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (strncmp (auxBuf,"A",1)==0)
				printf("Status: Data Valid\n");
			else if (strncmp (auxBuf,"V",1)==0)
				printf("Status: Void\n");
			else {
				printf("Invalid Status\n");
				return;
			}
		}
	}

	// Checksum
	if(buf[0]!='*')
		printf("Not valid GPGLL sentence format. Terminal '*' missing");
	else
		printf("\n**********GPGLL format string parsing complete**********\n");
}

/**
 *  GPGSVparser                          
 * <p>
 * This function is used for parsing GPGSV format sentences.
 * <p> 
 * 
 * @param  buf Buffer containing the input string read from the file
 * @param  bufsize Size of the input Buffer
 */

void GPGSVParser(char *buf, unsigned int bufSize) {
	char auxBuf[20];
	char* p2;
	unsigned int index=7;
	int i;
	printf("\n\n**********Parsing GPGSV input string**********\n\n");
	// GSV
	buf+=7;
	
	// Total number of messages of this type in this cycle
	if(index>=bufSize)
	{
		printf("End of string");
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Total number of messages not specified\n");
			return;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,0)<0) {
				printf("Total number of messages must be a number");
				return;
			}
			printf("Total number of messages of this type in this cycle: %s\n", auxBuf);
		}
	}
	// Message Number
	if(index>=bufSize)
	{
		printf("End of string");
		
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Message number not specified\n");
			
			return;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,0)<0) {
				printf("Message number must be a number");
				return;
			}
			printf("Message number: %s\n", auxBuf);
		}
	}

	// Total number of Satellites in view
	if(index>=bufSize)
	{
		printf("End of string");
		
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Number of Satellites not specified\n");
			buf++;
			index++;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,0)<0) {
				printf("Number of Satellites must be a number");
				return;
			}
			int q=atoi(auxBuf);
			if (q<0)
			{
				printf("Number of satellites in view canot be negative'\n");
				
				return;
			}
			if (q>12)
			{
				printf("Number of satellites in view canot be greater than 12'\n");
								
				return;
			}
			printf("Number of satellites being tracked: %d\n", q);
		}
	}

	for(i=0;i<4;i++)
	{
		printf("Information about SV # %d\n",i);
		if(index>=bufSize)
		{
			printf("End of string");
			
			return;
		}
		if((p2=strchr(buf,','))==NULL) {	
			printf("Wrong format\n");
			
			return;
		}
		else {
			if(p2-buf==0) {
				printf("	PRN number Satellite # %d: not specified\n",i);
				buf++;
				index++;
			}
			else {
				index+=(p2-buf+1);
				strncpy(auxBuf, buf, p2 - buf);
				auxBuf[p2 - buf] = '\0';
				buf = p2 + 1;
				if (checkdigit(auxBuf,0)<0) {
				printf("PRN number Satellite must be a number");
				return;
			}
			printf("	PRN number Satellite # %d: %s\n", i,auxBuf);
			}
		}

		if(index>=bufSize)
		{
			printf("End of string");
			
			return;
		}
		if((p2=strchr(buf,','))==NULL) {	
			printf("Wrong format\n");
			
			return;
		}
		else {
			if(p2-buf==0) {
				printf("	Elevation for satellite (in degrees) # %d : not specified\n",i);
				buf++;
				index++;
			}
			else {
				index+=(p2-buf+1);
				strncpy(auxBuf, buf, p2 - buf);
				auxBuf[p2 - buf] = '\0';
				buf = p2 + 1;
				if (checkdigit(auxBuf,0)<0) {
				printf("Elevation for satellite must be a number");
				return;
			}
				int ele = atoi(auxBuf);
				if (ele>90 || ele<0) {
					printf("Wrong value of Elevation. elevation measurements range from 0B0 to 90B0.");
					
					return;
				}
			printf("	Elevation for satellite (in degrees) # %d : %s\n",i,auxBuf);
			}
		}

		if(index>=bufSize)
		{
			printf("End of string");
			
			return;
		}
		if((p2=strchr(buf,','))==NULL) {	
			printf("Wrong format\n");
			
			return;
		}
		else {
			if(p2-buf==0) {
				printf("	Azimuth for satellite (degrees from True North) # %d : not specified\n",i);
				buf++;
				index++;
			}
			else {
				index+=(p2-buf+1);
				strncpy(auxBuf, buf, p2 - buf);
				auxBuf[p2 - buf] = '\0';
				buf = p2 + 1;
				if (checkdigit(auxBuf,0)<0) {
					printf("Azimuth for satellite must be a number");
					return;
				}
				int azi = atoi(auxBuf);
				if (azi>359 || azi<0) {
					printf("Wrong value of Azimuth. Azimuth measurements range from 0 to 359.");
					return;
				}
			printf("	Azimuth for satellite (degrees from True North) # %d : %s\n",i,auxBuf);
			}
		}

		if(index>=bufSize)
		{
			printf("End of string");
			
			return;
		}
		if((p2=strchr(buf,','))==NULL) {	
			printf("Wrong format\n");
			
			return;
		}
		else {
			if(p2-buf==0) {
				printf("	SNR for satellite # %d : not specified\n",i);
				buf++;
				index++;
			}
			else {
				index+=(p2-buf+1);
				strncpy(auxBuf, buf, p2 - buf);
				auxBuf[p2 - buf] = '\0';
				buf = p2 + 1;
				if (checkdigit(auxBuf,0)<0) {
					printf("SNR for satellite must be a number");
					return;
				}
				int snr = atoi(auxBuf);
				if (snr>99 || snr<0) {
					printf("Wrong value of SNR. SNR measurements range from 0 to 99.");
					
					return;
				}
			printf("	SNR for satellite # %d : %s\n",i,auxBuf);
			}
		}
	}
	// Checksum
	if(buf[0]!='*')
		printf("Not valid GPGSV sentence format. Terminal '*' missing");
	else
		printf("\n**********GPGSV format string parsing complete**********\n");
	
}

/**
 *  GPGSTparser                          
 * <p>
 * This function is used for parsing GPGST format sentences.
 * <p> 
 * 
 * @param  buf Buffer containing the input string read from the file
 * @param  bufsize Size of the input Buffer
 */

void GPGSTParser(char *buf, unsigned int bufSize) {
	char auxBuf[20];
	char* p2;
	unsigned int index=7;

	printf("\n\n**********Parsing GPGST input string**********\n\n");
	// GST
	buf+=7;
	if(index>=bufSize)
	{
		printf("End of string");
		
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Fix time not specified\n");
			buf++;
			index++;
		}
		else {
			index+=(p2-buf+1);
			printf("Fix taken at ");
			strncpy(auxBuf, buf, 2);
			auxBuf[2] = '\0';
			int hour = atoi(auxBuf);
			if (hour>23) {
				printf("Wrong value of time. Hour cannot be greater than 23");
				
				return;
			}
			buf += 2;
			printf("%s:",auxBuf);
			strncpy(auxBuf, buf, 2);
			auxBuf[2] = '\0';
			buf += 2;
			int min = atoi(auxBuf);
			if (min>59) {
				printf("Wrong value of time. Minutes cannot be greater than 59");
				
				return;
			}
			printf("%s:",auxBuf);
			strncpy(auxBuf, buf, 2);
			auxBuf[2] = '\0';
			buf = p2 + 1;
			int sec = atoi(auxBuf);
			if (sec>59) {
				printf("Wrong value of time. Seconds cannot be greater than 59");
				
				return;
			}
			printf("%s UTC\n",auxBuf);
		}
	}
	
	// RMS value of the pseudorange residuals
	if(index>=bufSize)
	{
		printf("End of string");
		
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		
		return;
	}
	else {
		if(p2-buf==0) {
			printf("RMS value of the pseudorange residuals not specified\n");
			buf++;
			index++;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,1)<0) {
				printf("RMS value must be a number");
				return;
			}
			printf("RMS value of the pseudorange residuals: %s\n", auxBuf);
		}
	}

	// Error ellipse semi-major axis 1 sigma error
	if(index>=bufSize)
	{
		printf("End of string");
		
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Error ellipse semi-major axis 1 sigma error not specified\n");
			buf++;
			index++;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,1)<0) {
				printf("Error ellipse semi-major axis 1 sigma error must be a number");
				return;
			}
			printf("Error ellipse semi-major axis 1 sigma error: %s m\n", auxBuf);
		}
	}

	// Error ellipse semi-minor axis 1 sigma error
	if(index>=bufSize)
	{
		printf("End of string");
		
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Error ellipse semi-minor axis 1 sigma error not specified\n");
			buf++;
			index++;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,1)<0) {
				printf("Error ellipse semi-minor axis 1 sigma error must be a number");
				return;
			}
			printf("Error ellipse semi-minor axis 1 sigma error: %s m\n", auxBuf);
		}
	}

	// Error ellipse orientation
	if(index>=bufSize)
	{
		printf("End of string");
		
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Error ellipse orientation not specified\n");
			buf++;
			index++;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,1)<0) {
				printf("Error ellipse orientation must be a number");
				return;
			}
			int azi = atoi(auxBuf);
			if (azi>359 || azi<0) {
				printf("Wrong value of ellipse orientation. Measurements range from 0B0 to 359B0.");
				
				return;
			}
			printf("Error ellipse orientation (degrees from true north): %s\n", auxBuf);
		}
	}

	//Latitude 1 sigma error
	if(index>=bufSize)
	{
		printf("End of string");
		
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Latitude 1 sigma error not specified\n");
			buf++;
			index++;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,1)<0) {
				printf("Latitude 1 sigma error must be a number");
				return;
			}
			printf("Latitude 1 sigma error (m): %s\n", auxBuf);
		}
	}

	//Longitude 1 sigma error
	if(index>=bufSize)
	{
		printf("End of string");
		
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Longitude 1 sigma error not specified\n");
			buf++;
			index++;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,1)<0) {
				printf("Longitude 1 sigma error must be a number");
				return;
			}
			printf("Longitude 1 sigma error (m): %s\n", auxBuf);
		}
	}

	//Height 1 sigma error
	if(index>=bufSize)
	{
		printf("End of string");
		
		return;
	}
	if((p2=strchr(buf,','))==NULL) {	
		printf("Wrong format\n");
		
		return;
	}
	else {
		if(p2-buf==0) {
			printf("Height 1 sigma error not specified\n");
			buf++;
			index++;
		}
		else {
			index+=(p2-buf+1);
			strncpy(auxBuf, buf, p2 - buf);
			auxBuf[p2 - buf] = '\0';
			buf = p2 + 1;
			if (checkdigit(auxBuf,1)<0) {
				printf("Height 1 sigma error must be a number");
				return;
			}
			printf("Height 1 sigma error (m): %s\n", auxBuf);
		}
	}

	// Checksum
	if(buf[0]!='*')
		printf("Not valid GPGST sentence format. Terminal '*' missing");
	else
		printf("\n**********GPGST format string parsing complete**********\n");
	
}

/**
 * ReadLineFromFile                          
 * <p>
 * This function is used to read one line from the input file from current position of file pointer.
 * <p> 
 * 
 * @param  fp File pointer pointing to current location in file
 * @param  line Data read from file is copied into this buffer
 * @return flag specifying success or failure
 */

int ReadLineFromFile(FILE *fp, char *line){
int i=0,inputSize=0;
	if(feof(fp)){
	return -1;
	}
	while(!feof(fp)){
		int ch;
		ch = fgetc(fp);
		if(ch==EOF && inputSize == 0){
			return -1;
		}
		else if(ch==EOF || ch == '\n'){
			inputSize++;
			line[i]='\0'; 
			return 0;
		}
		else {
			inputSize++;
			if(inputSize > MAX_INPUT_LINE_LENGTH)
			return inputSize; 
			line[i++]=ch; 
		}
	}
	line[i]='\0'; 
	return 0;
}

/**
 * SanitizeInput                         
 * <p>
 * This function removes white spaces from input string.
 * <p> 
 * 
 * @param  input Buffer containing the input string passed as input as well as processed output. 
 */
void SanitizeInput(char *input) {
	int i=0,j=0;
	while(input[j] != '\0') {
		if(isspace(input[j])){
		//if(input[j]) {
			j++;
		}
 		else {
			input[i] = input[j];
			i++;
			j++;
 		}
	}
	input[i]='\0';
}

/**
 * CheckInputExceptions                         
 * <p>
 * This function checks the input string for various irregullarities in format.
 * <p> 
 * 
 * @param  input Buffer containing the input string. 
 * @return value, 0 if no error found, else location of first error.
 */
int CheckInputExceptions(char *input){
	int i=0;
	int digitFound=0;
	int starFound=0;
	int DirectionFound=0;
	int dotFound=0;
	int HexDigit=0;
	int ManualFound=0;
	int AutoFound=0;
	int VFound=0;
	if(input[0] != ',')
		return 1;
	while(input[i] != '\0') {
		switch(input[i]){
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if((DirectionFound > 0) || ((starFound >0) && ((HexDigit+digitFound)>=2))){
					printf("\nInvalid Input ");
					return i;
				}
				digitFound++;
				break;		 
			case '.':
				if(dotFound > 0){
					printf("\nInvalid Input ");
					return i;
				}
				dotFound++;
				break;		 
			case ',':
				digitFound=0;
				starFound=0;
				DirectionFound=0;
				dotFound=0;
				HexDigit=0;
				break;

			case '*':
				if(starFound > 0){
					printf("\nInvalid Input ");
					return i;
				}
				starFound++;
				digitFound=0;
				break;		 
			case 'A':
				if((digitFound==0) && (HexDigit==0) && (DirectionFound==0) && (dotFound==0) && (starFound==0) && (AutoFound==0)) {
					AutoFound++;
					break;
				}
				if(starFound == 0){
					printf("\nInvalid Input ");
					return i;
				}
				if(HexDigit >=2){
					printf("\nInvalid Input ");
					return i;
				}
				HexDigit++;
				break;		 
			case 'B':
			case 'C':
			case 'D':
				if(starFound == 0){
					printf("\nInvalid Input ");
					return i;
				}
				if(HexDigit >=2){
					printf("\nInvalid Input ");
					return i;
				}
				HexDigit++;
				break;		 
			case 'E':
				if(starFound == 0){
					if(DirectionFound>0){
						printf("\nInvalid Input ");
						return i;
					}
					else {
						if((digitFound==0) && (HexDigit==0) && (DirectionFound==0)){
							DirectionFound++;
							break;
						}
						else {
							printf("\nInvalid Input ");
							return i;
						}		
					}
				}
				if(HexDigit >=2){
					printf("\nInvalid Input ");
					return i;
				}
				HexDigit++;
				break;		 
			case 'F':
				if(starFound == 0){
					printf("\nInvalid Input ");
					return i;
				}
				if(HexDigit >=2){
					printf("\nInvalid Input ");
					return i;
				}
				HexDigit++;
				break;		 
		
			case 'a':
				if((digitFound==0) && (HexDigit==0) && (DirectionFound==0) && (dotFound==0) && (starFound==0) && (AutoFound==0)) {
					AutoFound++;
					break;
				}
				if(starFound == 0){
					printf("\nInvalid Input ");
					return i;
				}
				if(HexDigit >=2){
					printf("\nInvalid Input ");
					return i;
				}
				HexDigit++;
				break;		 
			case 'b':
			case 'c':
			case 'd':
				if(starFound == 0){
					printf("\nInvalid Input ");
					return i;
				}
				if(HexDigit >=2){
					printf("\nInvalid Input ");
					return i;
				}
				HexDigit++;
				break;		 
			case 'e':
				if(starFound == 0){
					if(DirectionFound>0){
						printf("\nInvalid Input ");
						return i;
					}
					else{
						if((digitFound==0)  && (HexDigit==0) && (DirectionFound==0)){
							DirectionFound++;
							break;
						}
						else{
							printf("\nInvalid Input ");
							return i;
						}		
					}
				}
				if(HexDigit >=2){
					printf("\nInvalid Input ");
					return i;
				}
				HexDigit++;
				break;		 
			case 'f':
				if(starFound == 0){
					printf("\nInvalid Input ");
					return i;
				}
				if(HexDigit >=2){
					printf("\nInvalid Input ");
					return i;
				}
				HexDigit++;
				break;		 

			case 'm':
				if((digitFound==0) && (HexDigit==0) && (DirectionFound==0) && (dotFound==0) && (starFound==0)){
					ManualFound++;
				}
				else{
					printf("\nInvalid Input ");
					return i;
				}		
			case 'v':
				if((digitFound==0) && (HexDigit==0) && (DirectionFound==0) && (dotFound==0) && (starFound==0) && (AutoFound==0)&&(VFound==0)){
					VFound++;
					break;
				}
				else{
					printf("\nInvalid Input ");
					return i;
				}		
			case 'M':
				if((digitFound==0) && (HexDigit==0) && (DirectionFound==0) && (dotFound==0) && (starFound==0)){
					ManualFound++;
				}
				else{
					printf("\nInvalid Input ");
					return i;
				}		
		
			case 'N':
			case 'S':
				if((digitFound==0) && (starFound==0) && (HexDigit==0) && (DirectionFound==0)){
					DirectionFound++;
					break;
				}
				else{
					printf("\nInvalid Input ");
					return i;
				}		
			case 'V':
				if((digitFound==0) && (HexDigit==0) && (DirectionFound==0) && (dotFound==0) && (starFound==0) && (AutoFound==0)&&(VFound==0)){
					VFound++;
					break;
				}
				else{
					printf("\nInvalid Input ");
					return i;
				}		

			case 'W':	
				if((digitFound==0) && (starFound==0) && (HexDigit==0) && (DirectionFound==0)){
					DirectionFound++;
					break;
				}
				else{
					printf("\nInvalid Input ");
					return i;
				}		
		
			default:
				printf("\nInvalid Input ");
				return i;
		}
		i++;
	}
	return 0;
}




int main(){
	char input[MAX_INPUT_LINE_LENGTH+1],auxBuf[10];
	FILE *fp;
	int retLength=0;
	fp=fopen("message.txt","r");
	if(fp==NULL)
	{ 
		printf("Unable to open the file\n");
		return -1; 
	}
	while(((retLength=ReadLineFromFile(fp, input))>=0))
	{
		
		if(retLength > MAX_INPUT_LINE_LENGTH){
			printf("\nThe Input Size exceeded the max length %d",MAX_INPUT_LINE_LENGTH);
			fclose(fp);
			return -1; 
		}
		SanitizeInput(input);
		strncpy(auxBuf, input, 6);
		auxBuf[6]='\0';
		if(input[0] != '$') {
			printf("Not a valid NMEA sentence. Sentense should start with '$'");
			fclose(fp);
			return -1;
		}
		char* p1;
		int errorLoc;
		p1=strtok(auxBuf,",");
		
		if((errorLoc=CheckInputExceptions((input+6)))>0){
		printf("\n%s",input);
		printf("\nError Found at location :%d\n",(errorLoc+7));
		}
		else if(strcmp(p1, "$GPGGA") == 0){
			GPGGAParser(input, strlen(input));
		}
		else if(strcmp(p1, "$GPGSV") == 0){
			GPGSVParser(input, sizeof(input));
		}
		else if(strcmp(p1, "$GPGSA") == 0){
			GPGSAParser(input, sizeof(input));
		}
		else if(strcmp(p1, "$GPGLL") == 0){
			GPGLLParser(input, sizeof(input));
		}
		else if(strcmp(p1, "$GPGST") == 0){
			GPGSTParser(input, sizeof(input));
		}
		else
			printf("Format not supported");
		printf("\n");
	}
	fclose(fp);
	return 0;
}


