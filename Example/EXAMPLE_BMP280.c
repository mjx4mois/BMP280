/*-----------------------------------------------------------------------
     Creator		: Morris chiou
     Sensor		: BMP280  pressure & temperature sensor
     File Name		: EXAMPLE_BMP280.c
     Function		: EXAMPLE_BMP280
     Create Date	: 2020/05/31
---------------------------------------------------------------------- */
#ifndef __BMP280_EXAMPLE__
#define __BMP280_EXAMPLE__

#include <stdio.h>
#include <delay.h>
#include <math.h>
#include <i2c.h>
#include <alcd.h>
#include "SENSOR_BMP280.h"
#include "Porting_Layer.h"

BMP280_PRESS_TEMPREATURE_DATA	BMP280_RAW_DATA,BMP280_DATA;	/*raw data & final data	*/
BMP280_CALCULATION_PARAMETER 	BMP280_CALIB_DATA;				/*Calibration Coefficients */

void EXAMPLE_BMP280(void);

void EXAMPLE_BMP280(void)
{

	CHAR8S status = 0;
	FLOAT final_atmosphere,final_sealevel_press,final_altitude;
	CHAR8U id = 0;
      	INT32U data[5]={0}; 
	INT32U lcd_temp[3]={0},lcd_data[6]={0}; 	
	INT32S display_temp,display_press;
	FLOAT display_altitude;
	
		/*set BMP280 software reset*/
		BMP280_SET_RESET();
		if(status!=0)
		{
			printf("reset BMP280 fail\r\n");
		}
		else
		{
			printf("reset BMP280\r\n");
		}

		delay_ms(30); /* tiny delay*/

			
		/*get BMP280 ID*/
		status = BMP280_GET_ID(&id);
		if(status!=0)
		{
			printf("read BMP280 fail\r\n");
		}
		else
		{
			printf("BMP280 id : 0x%x\r\n",id);
		}
			
		
		/*initial the BMP280 , read calibration data*/
		status = BMP280_SET_INITAIL(&BMP280_CALIB_DATA);
		if(status!=0) 
		{
			printf("BMP280 initial fail\r\n");
		}
		else
		{
			printf("BMP280 initial ok\r\n");
		}

		while(1)
		{

			/*----------------------------- GET THE VALUE SEQUENCE ----------------------------*/
			/*
				set the temperature & pressure mode & system mode
				BMP280_TEMPERAUTRE_MODE	= BMP280_TEMP_16BIT
				BMP280_PRESS_MODE			= BMP280_PRESS_18BIT
				BMP280_SYS_MODE			= BMP280_SYS_FORCED_MODE
			*/

			/* set the BMP280 power mode & temperature mode & press mode */
			status=BMP280_SET_MEASUREMENT_SYS_MODE(BMP280_OP_MODE_SET);
			if(status!=0) 
			{
				printf("set BMP280 mode fail\r\n");
			}
			else
			{
				printf("set BMP280 mode ok\r\n");		
			}
	
			/* read raw data*/
			status = BMP280_GET_RAW_DATA(&BMP280_RAW_DATA);
			if(status!=0) 
			{
				printf("get BMP280 raw fail\r\n");
			}
			else
			{
				printf("get BMP280 raw ok\r\n");		
			}

	
			/*	to calculate Temperature & pressure data */
			/* 
				get final temperature & pressure data unit :
				 FINA_TEMP=5123 -> 51.23 C
				 FINA_PRESSURE = 96386 -> 963.86hpa
			*/
			status = BMP280_GET_CALCULATE_DATA(&BMP280_DATA,BMP280_RAW_DATA,BMP280_CALIB_DATA);
			if(status!=0) 
			{
				printf("calculate BMP280 final data fail\r\n");					
			}
			else
			{
				printf("calculate BMP280 final data ok\r\n");	
			}

			/* Calculate the altitude */		
			BMP280_GET_ALTITUDE(BMP280_DATA.final_press,&final_altitude);

			/* Calculate the sea level pressure */
			BMP280_GET_SEALEVEL_PRESSURE(final_altitude,BMP280_DATA.final_press,&final_sealevel_press);
			
			printf("temperature : %f C\r\n",BMP280_DATA.final_temperature);
			printf("final pressure : %f  hPa\r\n",BMP280_DATA.final_press );						
			printf("Altitude    : %f m\r\n",final_altitude/100);	/* need  /100 */
			printf("Sea leve press   : %f Pa\r\n",final_sealevel_press/100);	/* need  /100 */



			/* for display */
			display_temp = (INT32S) (BMP280_DATA.final_temperature *100);
			display_press = (INT32S) (BMP280_DATA.final_press *100);
			display_altitude = final_altitude/100;/* need  /100 */
			
			/* BMP280 temperature */                    
			lcd_gotoxy(0,0);
			lcd_putsf("Temp:");     

			if(BMP280_DATA.final_temperature<0)
			{
				lcd_putchar(45);
			}                             
			else
			{
				lcd_putchar(32);
			}
			
			/* final_temperature unit : 0.1 C -> if  final_temperature == 2457 , 24.57 C */
			lcd_temp[0]=(INT32U) ((INT32U)display_temp/1000)%10;
			lcd_temp[1]=(INT32U) ((INT32U)display_temp/100)%10;
			lcd_temp[2]=(INT32U) ((INT32U)display_temp/10)%10;
			lcd_temp[3]=(INT32U) ((INT32U)display_temp%10);
			/* show final_temperature data on LCD */
			lcd_putchar(48+lcd_temp[0]);
			lcd_putchar(48+lcd_temp[1]);
			lcd_putchar(46);	/* show "."*/	
			lcd_putchar(48+lcd_temp[2]);
			lcd_putchar(48+lcd_temp[3]);		
			lcd_putchar(67);	/* show "C"*/	


	            	/* BMP280 Pressure */                    
			lcd_gotoxy(0,1);
			lcd_putsf("P:");    
			/* final_pressure unit : Pa -> if  final_pressure == 99687 Pa , 996.87hPa */
			lcd_data[0]=((INT32S)display_press/100000)%10;
			lcd_data[1]=((INT32S)display_press/10000)%10;
			lcd_data[2]=((INT32S)display_press/1000)%10;
			lcd_data[3]=((INT32S)display_press/100)%10;	
			lcd_data[4]=((INT32S)display_press/10)%10;
			lcd_data[5]=((INT32S)display_press)%10;
			/* show final_pressure data on LCD */
			lcd_putchar(48+lcd_data[0]);
			lcd_putchar(48+lcd_data[1]);
			lcd_putchar(48+lcd_data[2]);
			lcd_putchar(48+lcd_data[3]);
			lcd_putchar(46);	/* show "."*/	
			lcd_putchar(48+lcd_data[4]);	
			lcd_putchar(48+lcd_data[5]);	   
			lcd_putsf("hPa");/* show "hPa"*/	


			/* BMP280 Altitude */                    
			lcd_gotoxy(0,2);
			lcd_putsf("Alt:");    
			/* final_altitude unit : m -> if  final_altitude == 612 Pa , 612m */
			lcd_data[0]=((INT32S)display_altitude/1000)%10;
			lcd_data[1]=((INT32S)display_altitude/100)%10;	
			lcd_data[2]=((INT32S)display_altitude/10)%10;
			lcd_data[3]=((INT32S)display_altitude)%10;
			lcd_data[4]=(INT32S)(display_altitude*10)%10;
			lcd_data[5]=(INT32S)(display_altitude*100)%10;
			/* show final_altitude data on LCD */
			lcd_putchar(48+lcd_data[0]);
			lcd_putchar(48+lcd_data[1]);
			lcd_putchar(48+lcd_data[2]);
			lcd_putchar(48+lcd_data[3]);
			lcd_putchar(46);	/* show "."*/	
			lcd_putchar(48+lcd_data[4]);	
			lcd_putchar(48+lcd_data[5]);   
			lcd_putsf("m");/* show "m"*/	


			/* BMP280 sea leve pressure*/                    
			lcd_gotoxy(0,3);
			lcd_putsf("P0:");    /* P0 -> Sea leve pressure */
			/* final_sealevel_press unit : Pa -> if  final_sealevel_press == 99687 Pa , 996.87hPa */
			lcd_data[0]=((INT32S)final_sealevel_press/100000);
			lcd_data[1]=((INT32S)final_sealevel_press/10000)%10;
			lcd_data[2]=((INT32S)final_sealevel_press/1000)%10;
			lcd_data[3]=((INT32S)final_sealevel_press/100)%10;	
			lcd_data[4]=((INT32S)final_sealevel_press/10)%10;
			lcd_data[5]=((INT32S)final_sealevel_press)%10;
			/* show final_sealevel_press data on LCD */
			lcd_putchar(48+lcd_data[0]);
			lcd_putchar(48+lcd_data[1]);
			lcd_putchar(48+lcd_data[2]);
			lcd_putchar(48+lcd_data[3]);
			lcd_putchar(46);	/* show "."*/	
			lcd_putchar(48+lcd_data[4]);	
			lcd_putchar(48+lcd_data[5]);	   
			lcd_putsf("hPa");/* show "hPa"*/		
	

			delay_ms(100);

		}
}
		
#endif		//#ifndef __BMP280_EXAMPLE__
