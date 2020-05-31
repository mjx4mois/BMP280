/*-----------------------------------------------------------------------
     Creator		: Morris chiou
     Sensor		: pressure & temperature sensor
     File Name		: SENSOR_BMP280.h
     Function		: SENSOR_BMP280
     Create Date	: 2018/04/05
     
---------------------------------------------------------------------- */
#ifndef __BMP280_HEADER__ 
#define __BMP280_HEADER__  

#define BMP280_DEBUG		(0)			/*IF set "1" enable debug , print information*


/*
	******************************************** measurement time ********************************************
	Oversampling setting		Pressure sampling		Temperature			sampling	Measurement time[ms]			Measurement rate[HZ]
	Ultra low power				x1					x1						6.4ms(MAX)						155.6Hz(MAX)
	Low power					x2					x1						8.7ms(MAX)						114.6Hz(MAX)
	Standard resolution			x4					x1						13.3ms(MAX)						75.0Hz(MAX)
	High resolution				x8					x1						22.5ms(MAX)						44.4Hz(MAX)
	Ultra high resolution			x16					x2						43.2ms(MAX)						23.1Hz(MAX)
	******************************************** measurement time ********************************************
*/


/*--------------------------------- Define SLAVE ADDRESS -------------------------------------*/
/* BMP280 SLAVE ADDRESS*/
	#define BMP280_ADDR_PIN 	 		(0)			/*PIN5 SDO(ADDR) connect  "0" GND or "1" Vcc*/
#if (BMP280_ADDR_PIN == 1)
	#define BMP280_SLAVE_ADDRESS     	(0XEE)
#else if (BMP280_ADDR_PIN == 0)
	#define BMP280_SLAVE_ADDRESS     	(0XEC)
#endif
/*--------------------------------- Define SLAVE ADDRESS -------------------------------------*/



/* ------------- DEFINE BMP280 STRUCT  & Parameter ------------*/
/* ------------- DEFINE BMP280 STRUCT  ------------*/

#define DIG_T1_REG_ADDR 					(0x88)

typedef struct BMP280_CALCULATION_PARAMETER_S
{
	INT16U dig_T1;	/*form 0x88(LSB)&0x89(MSB)*/
	INT16S dig_T2;	/*form 0x8A(LSB)&0x8B(MSB)*/
	INT16S dig_T3;	/*form 0x8C(LSB)&0x8D(MSB)*/
	INT16U dig_P1;	/*form 0x8E(LSB)&0x8F(MSB)*/
	INT16S dig_P2;	/*form 0x90(LSB)&0x91(MSB)*/
	INT16S dig_P3;	/*form 0x92(LSB)&0x93(MSB)*/
	INT16S dig_P4;	/*form 0x94(LSB)&0x95(MSB)*/
	INT16S dig_P5;	/*form 0x96(LSB)&0x97(MSB)*/
	INT16S dig_P6;	/*form 0x98(LSB)&0x99(MSB)*/
	INT16S dig_P7;	/*form 0x9A(LSB)&0x9B(MSB)*/
	INT16S dig_P8;	/*form 0x9C(LSB)&0x9D(MSB)*/
	INT16S dig_P9;	/*form 0x9E(LSB)&0x9F(MSB)*/
	
}BMP280_CALCULATION_PARAMETER;


typedef struct BMP280_PRESS_TEMPREATURE_DATA_S
{
	INT32S press_data;			/*from 0xF7,0xF8,0xF9(7:4) */
	INT32S temperature_data;	/*from 0xFA,0xFB,0xFC(7:4) */
	FLOAT final_press;			/*final pressure data */
	FLOAT final_temperature;			/*final temperature data */

}BMP280_PRESS_TEMPREATURE_DATA;
/* ------------- DEFINE BMP280 STRUCT  ------------*/
/*  ------------- DEFINE BMP280 Parameter -------------*/


/*--------------------------------------------------------------------------*/
/*set register(0xF4) bit7:5 : temperature mode*/
#define BMP280_TEMP_SKIP					(0x00<<5)	/*SKIP measurement temperautre */
#define BMP280_TEMP_16BIT					(0x01<<5)	/*resolution:16bit ,oversampling:x1*/
#define BMP280_TEMP_17BIT					(0x02<<5)	/*resolution:17bit ,oversampling:x2*/
#define BMP280_TEMP_18BIT					(0x03<<5)	/*resolution:18bit ,oversampling:x4*/
#define BMP280_TEMP_19BIT					(0x04<<5)	/*resolution:19bit ,oversampling:x8*/
#define BMP280_TEMP_20BIT					(0x05<<5)	/*resolution:20bit ,oversampling:x16*/

/*set register(0xF4) bit4:2 : pressure mode */
#define BMP280_PRESS_SKIP					(0x00<<2)	/*SKIP measurement pressure */
#define BMP280_PRESS_16BIT					(0x01<<2)	/*resolution:16bit ,oversampling:x1*/
#define BMP280_PRESS_17BIT					(0x02<<2)	/*resolution:17bit ,oversampling:x2*/
#define BMP280_PRESS_18BIT					(0x03<<2)	/*resolution:18bit ,oversampling:x4*/
#define BMP280_PRESS_19BIT					(0x04<<2)	/*resolution:19bit ,oversampling:x8*/
#define BMP280_PRESS_20BIT					(0x05<<2)	/*resolution:20bit ,oversampling:x16*/

/*set register(0xF4) bit1:0 : system mode*/
#define BMP280_SYS_SLEEP_MODE			(0x00<<0)	/*system mode in sleep mode*/
#define BMP280_SYS_FORCED_MODE			(0x01<<0)	/*system mode in FORCED mode , measurement once & turn sleep mode*/
#define BMP280_SYS_NORMAL_MODE			(0x03<<0)	/*system mode in Normal , continue measurement*/
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/**** only BMP280_SYS_MODE == BMP280_SYS_NORMAL_MODE*/
/*in normal mode -> use standby time period to read data.*/
#define BMP280_STANDBY_TIME_0_5MS		(0x00<<5)	/*standby time : 0.5ms*/
#define BMP280_STANDBY_TIME_62_5MS		(0x01<<5)	/*standby time : 62.5ms*/
#define BMP280_STANDBY_TIME_125MS		(0x02<<5)	/*standby time : 125ms*/
#define BMP280_STANDBY_TIME_250MS		(0x03<<5)	/*standby time : 2505ms*/
#define BMP280_STANDBY_TIME_500MS		(0x04<<5)	/*standby time : 500ms*/
#define BMP280_STANDBY_TIME_1000MS		(0x05<<5)	/*standby time : 1000ms*/
#define BMP280_STANDBY_TIME_2000MS		(0x06<<5)	/*standby time : 2000ms*/
#define BMP280_STANDBY_TIME_4000MS		(0x07<<5)	/*standby time : 4000ms*/

/**** for function (BMP180_SET_STANDBY_IIR_FILTER_MODE)*/
#define BMP280_STANDBY_TIME_SET			(BMP280_STANDBY_TIME_62_5MS)

#define BMP280_IIR_FILTER_OFF				(0x00<<2)	/*IIR filter off	, bandwidth : FULL*/
#define BMP280_IIR_FILTER_2					(0x01<<2)	/*IIR 2			, bandwidth : 0.223 * ODR*/
#define BMP280_IIR_FILTER_4					(0x02<<2)	/*IIR 4			, bandwidth : 0.092 * ODR*/
#define BMP280_IIR_FILTER_8					(0x03<<2)	/*IIR 8			, bandwidth : 0.042 * ODR*/
#define BMP280_IIR_FILTER_16				(0x04<<2)	/*IIR 16			, bandwidth : 0.021 * ODR*/

/*** for function (BMP180_SET_STANDBY_IIR_FILTER_MODE)*/
#define BMP280_IIR_MODE						(BMP280_IIR_FILTER_16)		
/*--------------------------------------------------------------------------*/


/**** for function (BMP280_SET_MEASUREMENT_SYS_MODE)*/
#define BMP280_TEMPERAUTRE_MODE			(BMP280_TEMP_20BIT)
/**** for function (BMP280_SET_MEASUREMENT_SYS_MODE)*/
#define BMP280_PRESS_MODE					(BMP280_PRESS_20BIT)
/**** for function (BMP280_SET_MEASUREMENT_SYS_MODE)*/
#define BMP280_SYS_MODE					(BMP280_SYS_FORCED_MODE)

/*****************************/
/* SET the BMP280 operation mode */
#define BMP280_OP_MODE_SET				(BMP280_TEMPERAUTRE_MODE|BMP280_PRESS_MODE|BMP280_SYS_MODE)
/*****************************/

/* ------------- DEFINE BMP280 Parameter ------------- */
/* ------------- DEFINE BMP280 STRUCT  & Parameter ------------ */



/* ------------- DEFINE BMP280 REGISTER  ------------*/
/*-------------------------------------------------------------------*/
/* Temperautre raw data XLSB*/
#define BMP280_TEMPERAUTRE_XLSB_DATA			(0xFC)		
/*bit 7:4 -> XLSB DATA */
/*----------------------------*/
/*bit [3:0] -> "0" , Reserved.*/
/*-------------------------------------------------------------------*/
/* Temperautre raw data LSB*/
#define BMP280_TEMPERAUTRE_LSB_DATA				(0xFB)		
/*bit 7:0 -> LSB DATA */
/*-------------------------------------------------------------------*/
/* Temperautre raw data MSB*/
#define BMP280_TEMPERAUTRE_MSB_DATA			(0xFA)		
/*bit 7:0 -> MSB DATA*/
/*-------------------------------------------------------------------*/
/* Pressure raw data XLSB*/
#define BMP280_PRESSURE_XLSB_DATA				(0xF9)
/*bit 7:4 -> XLSB DATA */
/*----------------------------*/
/*bit [3:0] -> "0" , Reserved.*/
/*-------------------------------------------------------------------*/
/* Pressure raw data LSB*/
#define BMP280_PRESSURE_LSB_DATA					(0xF8)		
/*bit 7:0 -> LSB DATA */
/*-------------------------------------------------------------------*/
/* Pressure raw data MSB*/
#define BMP280_PRESSURE_MSB_DATA				(0xF7)		
/*bit 7:0 -> MSB DATA*/
/*-------------------------------------------------------------------*/
/* control BMP280 data speed*/
#define BMP280_CONFIG								(0xF5)
//bit 7:5 -> t_sb,set the standby time (only Normal).
//Controls inactive duration tstandby in normal mode
// 7 6 5    tstandby[ms] 
// 0 0 0		0.5
// 0 0 1		62.5
// 0 1 0 		125
// 0 1 1		250
// 1 0 0		500
// 1 0 1		1000
// 1 1 0		2000
// 1 1 1		4000
//----------------------------
//bit 4:2 -> filter,controls the time constant of IIR filter
// 4 3 2			Filter coefficient		Bandwidth
// 0 0 0			filter off				FULL
// 0 0 1			2						0.223 * ODR
// 0 1 0			4						0.092 * ODR
// 0 1 1			8						0.042 * ODR
// 1 0 0 ,other		16						0.021 * ODR
//----------------------------
//bit 0 -> Eanble 3-wire SPI,if set "1"

/*-------------------------------------------------------------------*/
/*
	control BMP280 measurement mode & system mode
	measurement mode : pressure sampling mode & temperature sampling mode
	system mode :	sleep mode , fource mode , normal mode

*/
#define BMP280_MEAS_MODE							(0xF4)
/*
	bit 7:5 set the temperature mode 
	7 6 5	temperature oversampling					Temperauture resoluttion
	0 0 0	Skip(output set to 0x80000)   					-
	0 0 1		x1										16bit / 0.0050 C
	0 1 0		x2										17bit / 0.0025 C						
	0 1 1		x4										18bit / 0.0012 C
	1 0 0		x8										19bit / 0.0006 C
	1 0 1		x16										20bit / 0.0003 C
	1 1 0		x16										20bit / 0.0003 C
	1 1 1		x16										20bit / 0.0003 C
	----------------------------
	bit 4:2 set the pressure mode
	4 3 2 	pressure oversampling				pressure resoluttion			*Recommand temperature oversampling
	0 0 0	Skip(output set to 0x80000)   				-
	0 0 1		x1								16bit / 2.62 Pa				x1
	0 1 0		x2								17bit / 1.31 Pa				x1				
	0 1 1		x4								18bit / 0.66 Pa				x1
	1 0 0		x8								19bit / 0.33 Pa				x1
	1 0 1		x16								20bit / 0.16 Pa				x2
	----------------------------
	bit 1:0 set BMP280 system mode
	1 0		system mode
	0 0		SLEEP mode
	0 1 		Forced mode				// only measurement once. when measurement finished, return sleep mode
	1 0		Forced mode
	1 1		Normal mode 	
*/
/* -------------------------------------------------------------------*/
/* BMP280 status*/
#define BMP280_STATUS								(0xF3)
/* 
	bit 3 ->	measurment -> automatically set "1" whenever a conversion is running!
			if "0" , the results have been transferred to the data registers!
	----------------------------
	bit 0 -> im_update -> automatically set to "1" when the NVM data are being 
			copied to image register, and back to "0" whe the copying is done
			-> if "0" ,user can read the NVM data[Compensation parameter data 0x88~0x9F]
*/     
/* -------------------------------------------------------------------*/
/*  reset the BMP280 */
#define BMP280_RESET									(0xE0)
/* write "0xB6" to this register reset the BMP280.*/
/*  writing other value than 0xB6 has no effect. Readout value is always 0x00.*/
/*-------------------------------------------------------------------*/
/* control BMP280 data speed*/
#define BMP280_REG_ID								(0xD0)
/*ID number : 0x58 , this number can be read as soon as the BMP280 finished the power on reset.*/
/*-------------------------------------------------------------------*/

/****** 0X88 ~ 0X9F register -> calculation of pressure & temperautre for BMP280*/

/* ------------- DEFINE BMP280 REGISTER  ------------*/




//********************************************* SYSTEM *************************************************
/*--------------------------------------------------------------------------------------------------*/
/*
	initial BMP280  
	1.check status bit 0 ==0? yes:Read calibration data ;No: wait status bit 0 ==0
	2.read calibration data.
*/
CHAR8S BMP280_SET_INITAIL(BMP280_CALCULATION_PARAMETER *para_data);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* get BMP280 ID */
CHAR8S BMP280_GET_ID(CHAR8U *BMP280_ID);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* 
	reset BMP280 
	write 0xB6 to 0xE0
	 if read this register 0xE0 , the value always 0x00.
*/
CHAR8S BMP280_SET_RESET(void);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
// get the BMP280 status , read register 0xF3 
// check bit 3 & 0 .
// ** before read data , the status register bit3 & bit0 must be "0"!!
CHAR8S BMP280_GET_STATUS(CHAR8U *REG_STATUS);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
// set temperature measurement mode & pressure measurement mode & system mode
// use define parameter : BMP280_TEMPERAUTRE_MODE , BMP280_PRESS_MODE , BMP280_SYS_MODE
// Example :  BMP180_SET_MEASUREMENT_SYS_MODE(BMP280_TEMPERAUTRE_MODE|BMP280_PRESS_MODE|BMP280_SYS_MODE);
CHAR8S BMP280_SET_MEASUREMENT_SYS_MODE(CHAR8U SET_MODE);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
// set the Normal mode need Tstandby time & IIR filter mode
// use define parameter : BMP280_IIR_MODE , BMP280_STANDBY_TIME_SET
// Example :  BMP180_SET_MEASUREMENT_SYS_MODE(BMP280_STANDBY_TIME_SET|BMP280_IIR_MODE);
CHAR8S BMP280_SET_STANDBY_IIR_FILTER_MODE(CHAR8U SET_FILTER_MODE);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
// set measurment mode & system mode to read raw data(temperature & pressure).
// register 0x88~0X9F -> calibration data
CHAR8S BMP280_GET_CALIBRATION_DATA(BMP280_CALCULATION_PARAMETER *CAL_DATA);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
// set measurment mode & system mode to read raw data(temperature & pressure).
// temperature data : 0xFA(MSB)~0xFC(XLSB)
// pressure data 	: 0xF7(MSB)~0xF9(XLSB)
CHAR8S BMP280_GET_RAW_DATA(BMP280_PRESS_TEMPREATURE_DATA *RAW_DATA);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
// to calculate the temperature & pressure data
CHAR8S BMP280_GET_CALCULATE_DATA(BMP280_PRESS_TEMPREATURE_DATA *FINAL_DATA,BMP280_PRESS_TEMPREATURE_DATA RAW_DATA,BMP280_CALCULATION_PARAMETER CAL_DATA);
/*--------------------------------------------------------------------------------------------------*/
/* Calculate the sea level pressure */
void BMP280_GET_SEALEVEL_PRESSURE(FLOAT altitude,FLOAT pressure_data,FLOAT *sea_level_pressure);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* Calculate the altitude */
void BMP280_GET_ALTITUDE(FLOAT pressure_data,FLOAT *altitude_result);
/*--------------------------------------------------------------------------------------------------*/
//********************************************* SYSTEM *************************************************

#endif		 //#ifndef __BMP280_HEADER__  
