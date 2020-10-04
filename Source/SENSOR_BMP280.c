/*-----------------------------------------------------------------------
     Creator		: Morris chiou
     Sensor		: pressure & temperature sensor
     File Name		: SENSOR_BMP280.c
     Function		: SENSOR_BMP280
     Create Date	: 2018/04/06
     
---------------------------------------------------------------------- */

#include <stdio.h>
#include <math.h>
#include <delay.h>
#include <datatype_Layer.h>
#include <swi2c_Layer.h>
#include <SENSOR_BMP280.h>



#define SENSOR_BMP280_DEBUG		(0)			/*IF set "1" enable debug , print information*

/********************************************* SYSTEM **************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*
	initial BMP280 , check the status  
	1.check status bit 0 ==0? yes:Read calibration data ;No: wait status bit 0 ==0
	2.read calibration data.
*/
CHAR8S BMP280_SET_INITAIL(BMP280_CALCULATION_PARAMETER *para_data)
{
	CHAR8S status=0,fail_flag=0;
	CHAR8U count=0,read_data=0;

		/* check status , retry 10 */
		for(count=0;count<10;count++)
		{
			status = BMP280_GET_STATUS(&read_data);
			if(status==0 && (read_data & 0x01) == 0x00) /*check read success ? & status bit0 =0*/
			{
				fail_flag = 1;
				break;
			}
				delay_ms(5);
				fail_flag = -1;
		}

		if(fail_flag== (-1) )
		{
			return -1 ; /* BMP280 read status fail & cant read calibration data.*/
		}

		/*read calibration data */
		BMP280_GET_CALIBRATION_DATA(para_data);


		return 0;/*initial BMP280 success!!*/

}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* get BMP280 ID : 0x58*/
CHAR8S BMP280_GET_ID(CHAR8U *BMP280_ID)
{
	CHAR8S status=0;
	CHAR8U read_data;
	
		/* read BMP280 ID register */
		status = i2c_read_1_byte_data(BMP280_SLAVE_ADDRESS,BMP280_REG_ID,&read_data);
		if(status!=1)
		{
			return -1; /* read fail*/
		}
		
		*BMP280_ID = read_data ;/*& 0x09;//0x09 -> Mask*/
		return 0; /*read ID scucess*/	
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*
	reset BMP280 
	write 0xB6 to 0xE0
	if read this register 0xE0 , the value always 0x00.
*/
CHAR8S BMP280_SET_RESET(void)
{
	CHAR8S status=0;

		/*0xB6 -> reset command.*/
		status = i2c_write_1_byte_data(BMP280_SLAVE_ADDRESS,BMP280_RESET,0xB6);
		if(status!=1)
		{
			return -1; /*write fail*/
		}
		else
		{
			return 0;/*write reset command scucess*/
		}
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*
	get the BMP280 status , read register 0xF3 
	check bit 3 & 0 .
	before read data , the status register bit3 & bit0 must be "0"!!
*/
CHAR8S BMP280_GET_STATUS(CHAR8U *REG_STATUS)
{
	CHAR8S status=0;
	CHAR8U read_data;

		/* read BMP280 status register */
		status = i2c_read_1_byte_data(BMP280_SLAVE_ADDRESS,BMP280_STATUS,&read_data);
		if(status!=1)
		{
			return -1;	/*read fail*/
		}
		
		*REG_STATUS = read_data & 0x09;	/*0x09 -> Mask*/
		return 0;	/*read STATUS scucess*/

}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*
	set temperature measurement mode & pressure measurement mode & system mode
	use define parameter : BMP280_TEMPERAUTRE_MODE , BMP280_PRESS_MODE , BMP280_SYS_MODE
	 Example :  BMP280_SET_MEASUREMENT_SYS_MODE(BMP280_TEMPERAUTRE_MODE|BMP280_PRESS_MODE|BMP280_SYS_MODE);
*/
CHAR8S BMP280_SET_MEASUREMENT_SYS_MODE(CHAR8U SET_MODE)
{

	CHAR8S status=0;
	CHAR8U reg_status=0;
	
		/*check set normal mode ?*/
		if( (SET_MODE & 0x03) && 0x03)/*in normal mode*/ 
		{
			/* STANDBY TIME only normal */
			/*set the Tstand by time call function : BMP280_SET_STANDBY_IIR_FILTER_MODE*/
			/*use parameter */
			status = BMP280_SET_STANDBY_IIR_FILTER_MODE(BMP280_IIR_MODE | BMP280_STANDBY_TIME_SET);
			if(status!=0)
			{
				return -1;	/*write fail*/
			}

				
			/* set to normal mode , start measurement */
			/* normal mode is continuous measurement */
			status = i2c_write_1_byte_data(BMP280_SLAVE_ADDRESS,BMP280_MEAS_MODE,SET_MODE);
			if(status!=1)
			{
				return -1;/*write fail*/
			}

		}
		else if( (SET_MODE & 0x03) == 0x00)/*in sleep mode*/ 
		{
			/* set to sleep mode */		
			status = i2c_write_1_byte_data(BMP280_SLAVE_ADDRESS,BMP280_MEAS_MODE,SET_MODE);
			if(status!=1)
			{
				return -1;/*write fail*/
			}			

		}
		else  if( (SET_MODE & 0x03) == 0x01 || (SET_MODE & 0x03) == 0x10)/* in Forced mode */ 
		{
			/* set the Tstand by time call function : BMP280_SET_STANDBY_IIR_FILTER_MODE*/
			/* use parameter */
			status = BMP280_SET_STANDBY_IIR_FILTER_MODE(BMP280_IIR_MODE | BMP280_STANDBY_TIME_SET);
			if(status!=0)
			{
				return -1; /* write fail */
			}
			
			/* start force mode measurement */
			/* force mode is once measurement */
			status = i2c_write_1_byte_data(BMP280_SLAVE_ADDRESS,BMP280_MEAS_MODE,SET_MODE);
			if(status!=1)
			{
				return -1;/* write fail */
			}
		}

		return 0 ; /*write measurement mode command scucess */


}
//--------------------------------------------------------------------------------------------------
/*--------------------------------------------------------------------------------------------------*/
/*
	set the Normal mode need Tstandby time & IIR filter mode
	use define parameter : BMP280_IIR_MODE , BMP280_STANDBY_TIME_SET
	Example : BMP280_SET_MEASUREMENT_SYS_MODE(BMP280_STANDBY_TIME_SET|BMP280_IIR_MODE);
*/
CHAR8S BMP280_SET_STANDBY_IIR_FILTER_MODE(CHAR8U SET_FILTER_MODE)
{
	CHAR8S status=0;

		/* set the IIR filter & Standby time */
		status = i2c_write_1_byte_data(BMP280_SLAVE_ADDRESS,BMP280_CONFIG,SET_FILTER_MODE);
		if(status!=1)
		{
			return -1; /* write fail*/
		}
		else
		{
			return 0 ;/*write IIR filter & Tstandby time command scucess 	*/
		}

}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* set measurment mode & system mode to read raw data(temperature & pressure).*/
/* register 0x88~0X9F -> calibration data*/
CHAR8S BMP280_GET_CALIBRATION_DATA(BMP280_CALCULATION_PARAMETER *CAL_DATA)
{

	CHAR8S status=0;
	CHAR8U BMP_CAL_COFF_ARRAY[24]={0},count=0;

		for(count=0;count<24;count++)
		{	
			/*0x88 -> start calibration data register , total 24bytes*/
			status = i2c_read_1_byte_data(BMP280_SLAVE_ADDRESS,(DIG_T1_REG_ADDR+count),&BMP_CAL_COFF_ARRAY[count]);
			if(status!=1)
			{
				printf("read error in CAL_COEFF_NUMBER[%d] \r\n",count);	
				break;
			}
		}

		/*check final read */
		if(status!=1) 
		{
			return -1;	/*read Calibration Coefficients fail*/
		}	
		
		
		/*calculating Calibration Coefficients*/
		CAL_DATA->dig_T1 = (INT32U) ( (BMP_CAL_COFF_ARRAY[1]<<8) + BMP_CAL_COFF_ARRAY[0]);
		CAL_DATA->dig_T2 = (INT32S) ( (BMP_CAL_COFF_ARRAY[3]<<8) + BMP_CAL_COFF_ARRAY[2]);
		CAL_DATA->dig_T3 = (INT32S) ( (BMP_CAL_COFF_ARRAY[5]<<8) + BMP_CAL_COFF_ARRAY[4]);
		CAL_DATA->dig_P1 = (INT32U) ( (BMP_CAL_COFF_ARRAY[7]<<8) + BMP_CAL_COFF_ARRAY[6]);
		CAL_DATA->dig_P2 = (INT32S) ( (BMP_CAL_COFF_ARRAY[9]<<8) + BMP_CAL_COFF_ARRAY[8]);
		CAL_DATA->dig_P3 = (INT32S) ( (BMP_CAL_COFF_ARRAY[11]<<8) + BMP_CAL_COFF_ARRAY[10]);
		CAL_DATA->dig_P4 = (INT32S) ( (BMP_CAL_COFF_ARRAY[13]<<8) + BMP_CAL_COFF_ARRAY[12]);
		CAL_DATA->dig_P5 = (INT32S) ( (BMP_CAL_COFF_ARRAY[15]<<8) + BMP_CAL_COFF_ARRAY[14]);
		CAL_DATA->dig_P6 = (INT32S) ( (BMP_CAL_COFF_ARRAY[17]<<8) + BMP_CAL_COFF_ARRAY[16]);
		CAL_DATA->dig_P7 = (INT32S) ( (BMP_CAL_COFF_ARRAY[19]<<8) + BMP_CAL_COFF_ARRAY[18]);
		CAL_DATA->dig_P8 = (INT32S) ( (BMP_CAL_COFF_ARRAY[21]<<8) + BMP_CAL_COFF_ARRAY[20]);
		CAL_DATA->dig_P9 = (INT32S) ( (BMP_CAL_COFF_ARRAY[23]<<8) + BMP_CAL_COFF_ARRAY[22]);

#if SENSOR_BMP280_DEBUG
		
		for(count=0;count<24;count++)
		{
			printf("a[0x%X]=0x%X \r\n ",(0x88+count),BMP_CAL_COFF_ARRAY[count]);
		}
		 
		printf("\r\n");
		
		printf("dig_T1 = 0x%X(%d)\r\n", CAL_DATA->dig_T1,CAL_DATA->dig_T1);
		printf("dig_T2 = 0x%X(%d)\r\n", CAL_DATA->dig_T2,CAL_DATA->dig_T2);
		printf("dig_T3 = 0x%X(%d)\r\n", CAL_DATA->dig_T3,CAL_DATA->dig_T3);
		printf("dig_P1 = 0x%X(%d)\r\n", CAL_DATA->dig_P1,CAL_DATA->dig_P1);
		printf("dig_P2 = 0x%X(%d)\r\n", CAL_DATA->dig_P2,CAL_DATA->dig_P2);
		printf("dig_P3 = 0x%X(%d)\r\n", CAL_DATA->dig_P3,CAL_DATA->dig_P3);
		printf("dig_P4 = 0x%X(%d)\r\n", CAL_DATA->dig_P4,CAL_DATA->dig_P4);
		printf("dig_P5 = 0x%X(%d)\r\n", CAL_DATA->dig_P5,CAL_DATA->dig_P5);
		printf("dig_P6 = 0x%X(%d)\r\n", CAL_DATA->dig_P6,CAL_DATA->dig_P6);
		printf("dig_P7 = 0x%X(%d)\r\n", CAL_DATA->dig_P7,CAL_DATA->dig_P7);
		printf("dig_P8 = 0x%X(%d)\r\n", CAL_DATA->dig_P8,CAL_DATA->dig_P8);
		printf("dig_P9 = 0x%X(%d)\r\n", CAL_DATA->dig_P9,CAL_DATA->dig_P9);
	
#endif
		return 0;	/*read Calibration Coefficients scucess!!*/

}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*
	set measurment mode & system mode to read raw data(temperature & pressure).
	temperature data : 0xFA(MSB)~0xFC(XLSB)
 pressure data 	: 0xF7(MSB)~0xF9(XLSB)
*/
CHAR8S BMP280_GET_RAW_DATA(BMP280_PRESS_TEMPREATURE_DATA *RAW_DATA)
{
	
	CHAR8S status=0,fail_flag=0;
	CHAR8U reg_status=0,count=0,compare=0;
	CHAR8U temp_data[3]={0};
	INT32S temp_raw_data=0;

		/*
			check measurement to delay , let BMP280 convert the value!
			forced mode : general to delay 50mS
			after delay to check ststus bit3 ==0? if ==0 -> the data ready! , if ==1 -> the sensor convert the value.
			check if BMP280_SYS_MODE == Normal , check tstand by time & delay
		*/
		
		compare = BMP280_STANDBY_TIME_SET;
		if( (BMP280_SYS_MODE & BMP280_SYS_NORMAL_MODE) == BMP280_SYS_NORMAL_MODE)	/*normal delay*/
		{

			switch(compare)/* detail measurement time see spec Table 14. */
			{
				case BMP280_STANDBY_TIME_0_5MS :
					delay_ms(1);
					break;

				case BMP280_STANDBY_TIME_62_5MS :
					delay_ms(65);
					break;
							
				case BMP280_STANDBY_TIME_125MS :
					delay_ms(130);
					break;

				case BMP280_STANDBY_TIME_250MS :
					delay_ms(260);
					break;

				case BMP280_STANDBY_TIME_500MS :
					delay_ms(520);
					break;
					
				case BMP280_STANDBY_TIME_1000MS :
					delay_ms(1040);					
					break;	
						
				case BMP280_STANDBY_TIME_2000MS :
					delay_ms(2050);						
					break;	

				case BMP280_STANDBY_TIME_4000MS :
					delay_ms(4100);				
					break;							
			}
				
		}
		else /*forced mode delay*/ /* this total wait 50mS , detail measurement time see spec Table 13. */
		{
						delay_ms(50);

		}
	
		/*check status bit3*/
		for(count=0;count<10;count++)
		{
			/*check ststus bit 3*/
			status = BMP280_GET_STATUS(&reg_status);
			/*IF STSTUS BIT3 == 1 -> sensor conversion is running , if done -> "0"*/
			if(status==0 && (reg_status & 0x08) == 0x00) /*check read success ? & status bit3 =0*/
			{
				fail_flag = 1;
				break;
			}
				delay_ms(5);
				fail_flag = -1;
			}
		
			if(fail_flag==-1)
			{
				return -1;	/*BMP280 read status fail & cant read the temperautre & pressure data*/
			}

			/*--------------------- read temperautre ----------------------*/
			/*read temperature raw data*/
			status = i2c_read_n_byte_data(BMP280_SLAVE_ADDRESS,BMP280_TEMPERAUTRE_MSB_DATA,3,&temp_data[0]);
			if(status!=1)
			{
				return -1;	/*read fail*/
			}
			
			/*temp_data[0] -> MSB , temp_data[1] -> LSB , temp_data[2] -> XLSB*/
			temp_raw_data = (INT32S)(((INT32S)temp_data[0]<<12) | ((INT32S)temp_data[1]<<4) | ((INT32S)temp_data[2]>>4));

			/*set the temperature raw data to variable*/

			#if SENSOR_BMP280_DEBUG
			printf("Reg 0xFA = 0x%X;Reg 0xFB = 0x%X;Reg 0xFC = 0x%X; \r\n", temp_data[0],temp_data[1],temp_data[2]);
			printf("temperature raw data = 0x%lx(Dec:%ld)\r\n",temp_raw_data,temp_raw_data);
			#endif
			
			RAW_DATA->temperature_data = temp_raw_data;
			/*--------------------- read temperautre ----------------------*/


			/*--------------------- read pressure ----------------------*/
			/*read pressure raw data*/
			status = i2c_read_n_byte_data(BMP280_SLAVE_ADDRESS,BMP280_PRESSURE_MSB_DATA,3,&temp_data[0]);
			if(status!=1)
			{
				return -1;/*read fail*/
			}
			
			/*temp_data[0] -> MSB , temp_data[1] -> LSB , temp_data[2] -> XLSB*/
			temp_raw_data = (INT32S)(((INT32S)temp_data[0]<<12) | ((INT32S)temp_data[1]<<4) | ((INT32S)temp_data[2]>>4));

			#if SENSOR_BMP280_DEBUG
			printf("Reg 0xF7 = 0x%X;Reg 0xF8 = 0x%X;Reg 0xF9 = 0x%X; \r\n", temp_data[0],temp_data[1],temp_data[2]);
			printf("pressure raw data = 0x%lx(Dec:%ld)\r\n",temp_raw_data,temp_raw_data);
			#endif
			
			/*set the pressure raw data to variable*/
			RAW_DATA->press_data = temp_raw_data;
			/*--------------------- read pressure ----------------------*/

			return 0; /*finished read raw data*/

			
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* to calculate the temperature & pressure data*/
CHAR8S BMP280_GET_CALCULATE_DATA(BMP280_PRESS_TEMPREATURE_DATA *FINAL_DATA,BMP280_PRESS_TEMPREATURE_DATA RAW_DATA,BMP280_CALCULATION_PARAMETER CAL_DATA)
{
	INT32S final_temp_data;
	INT32U final_press_data;
	INT32S temp_raw_data,press_raw_data;
	INT32S X1=0,X2=0,X11,X12,X21,X22,X31,X32;
	INT32S Y1=0,Y2=0;	


		/*---------------------------- TEMPERATURE CALCULATE --------------------------*/
		temp_raw_data = RAW_DATA.temperature_data;
		press_raw_data = RAW_DATA.press_data;

		X11 =((temp_raw_data>> 3)- ((INT32S)CAL_DATA.dig_T1<< 1));
		X12 =((INT32S)CAL_DATA.dig_T2);
		X1 = (X11* X12)>> 11;

		X21=((temp_raw_data>> 4)- ((INT32S)CAL_DATA.dig_T1));
		X22=((temp_raw_data>> 4)- ((INT32S)CAL_DATA.dig_T1));
		X31=((INT32S)CAL_DATA.dig_T3);
		X2 = (((X21* X22)>>12)*X31)>>14;

		final_temp_data = X1 + X2;

		/*NOTE: example if FINA_TEMP=5123 -> 51.23 C*/
		/*FINAL TERMPERATURE DATA*/
		FINAL_DATA->final_temperature = (FLOAT)((final_temp_data * 5 +128) >>8)/100;
		#if SENSOR_BMP280_DEBUG
		printf("final temperature data = %f C\r\n",FINAL_DATA->final_temperature);
		#endif
		
		/*---------------------------- TEMPERATURE CALCULATE --------------------------*/


		/*---------------------------- PRESSURE CALCULATE --------------------------*/
		Y1 = (((INT32S)final_temp_data)>>1)- (INT32S)64000;
		Y2 = (((Y1>>2) * (Y1>>2)) >> 11 ) * ((INT32S)CAL_DATA.dig_P6);
		Y2 = Y2 + ((Y1*((INT32S)CAL_DATA.dig_P5))<<1);
		Y2 = (Y2>>2)+(((INT32S)CAL_DATA.dig_P4)<<16);
		Y1 = (((CAL_DATA.dig_P3 * (((Y1>>2) * (Y1>>2)) >> 13 )) >> 3) + ((((INT32S)CAL_DATA.dig_P2) * Y1)>>1))>>18;
		Y1 =((((32768+Y1))*((INT32S)CAL_DATA.dig_P1))>>15);
		if (Y1 == 0)
		{
			return -1; 	/* avoid exception caused by division by zero*/
		}
		final_press_data= (((INT32U)(((INT32S)1048576)-(INT32U)press_raw_data)-(Y2>>12)))*3125;
		if (final_press_data < 0x80000000)
		{
			final_press_data = (final_press_data<< 1) / ((INT32U)Y1);
		}
		else
		{
			final_press_data = (final_press_data/ (INT32U)Y1) * 2;
		}
		Y1 = (((INT32S)CAL_DATA.dig_P9) * ((INT32S)(((final_press_data>>3) * (final_press_data>>3))>>13)))>>12;
		Y2 = (((INT32S)(final_press_data>>2)) * ((INT32S)CAL_DATA.dig_P8))>>13;

		/*NOTE: example if FINA_PRESSURE = 96386 -> 963.86hpa*/
		/*FINAL PRESSURE DATA*/	
		final_press_data = (INT32U)((INT32S)final_press_data + ((Y1 + Y2 + CAL_DATA.dig_P7) >> 4));
	
		//FINAL_DATA->press_data = final_press_data;
		FINAL_DATA->final_press = (FLOAT)final_press_data/100;
		
		#if SENSOR_BMP280_DEBUG
		printf("final pressure data = %f  hPa\r\n",FINAL_DATA->final_press );
		#endif
		
		/*---------------------------- PRESSURE CALCULATE --------------------------*/	

		return 0; 	/*finished calculate temperature & pressure data*/

}
/*--------------------------------------------------------------------------------------------------*/
/* Calculate the sea level pressure */
void BMP280_GET_ALTITUDE(FLOAT pressure_data,FLOAT *altitude_result)
{
	FLOAT alt_temp1,alt_temp2,alt_temp3,alt_temp4,alt_temp5;
	
	        /* Calculating absolute altitude  */
	        alt_temp1 = ((FLOAT)pressure_data/101325);
	        alt_temp2 = (1/5.255);
	        alt_temp3 = pow(alt_temp1,alt_temp2);
	        alt_temp4 = (1 - alt_temp3);
	        alt_temp5 = 44330.0 *(alt_temp4);	/*altitude result*/
	      
	 	*altitude_result = alt_temp5;		/* Final altitude*/
}
/*--------------------------------------------------------------------------------------------------*/
/* Calculate the altitude */      
void BMP280_GET_SEALEVEL_PRESSURE(FLOAT altitude,FLOAT pressure_data,FLOAT *sea_level_pressure)
{
	FLOAT alt_temp6,alt_temp7,alt_temp8,alt_temp9,alt_temp10;
	
	        /* Calculating pressure at sea level  */
	        alt_temp6 = ((FLOAT)altitude/44330.0);
	        alt_temp7 = (1 - alt_temp6);
	        alt_temp8 = pow(alt_temp7,5.255);
	        alt_temp9 = (pressure_data /alt_temp8);
	        alt_temp10 = alt_temp9;      
	        
		*sea_level_pressure = alt_temp10;		/* Final sea level*/
}
/*--------------------------------------------------------------------------------------------------*/      
/********************************************* SYSTEM **************************************************/