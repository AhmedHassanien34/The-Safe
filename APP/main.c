#include <util/delay.h>

/********************************************************************************************/
/* Include libraries                                                                        */
/********************************************************************************************/
#include "BIT_MATH.h"
#include "STD_TYPES.h"

/********************************************************************************************/
/* Include Interface Files in MCAL                                                          */
/********************************************************************************************/
#include "DIO_interface.h"
#include "EEPROM_interface.h"

/********************************************************************************************/
/* Include Interface Files in HAL                                                           */
/********************************************************************************************/

#include "Svm_interface.h"
#include "KPD_interface.h"
#include "CLCD_interface.h"

#define EEPROM_Status_Flag        0x20
#define EEPROM_PASSWORD_LOCATION1 0x21
#define EEPROM_PASSWORD_LOCATION2 0x22
#define EEPROM_PASSWORD_LOCATION3 0x23
#define EEPROM_PASSWORD_LOCATION4 0x24
#define First_Use                 0XFF
#define Entered_The_Safe          0x00
#define Max_Tries                 3

void main(void)
{
	CLCD_voidInit();
	KPD_voidInit();
	SERVO_voidTimer1InitOCR1AB();

	DIO_u8SetPinDirection(DIO_u8PORTB,DIO_u8PIN0,DIO_u8PIN_OUTPUT);
	DIO_u8SetPinDirection(DIO_u8PORTB,DIO_u8PIN1,DIO_u8PIN_OUTPUT);
	DIO_u8SetPinDirection(DIO_u8PORTB,DIO_u8PIN2,DIO_u8PIN_OUTPUT);
	DIO_u8SetPinDirection(DIO_u8PORTB,DIO_u8PIN3,DIO_u8PIN_OUTPUT);

	DIO_u8SetPinValue(DIO_u8PORTB,DIO_u8PIN0,DIO_u8PIN_LOW);
	DIO_u8SetPinValue(DIO_u8PORTB,DIO_u8PIN1,DIO_u8PIN_HIGH);
	DIO_u8SetPinValue(DIO_u8PORTB,DIO_u8PIN2,DIO_u8PIN_LOW);
	DIO_u8SetPinValue(DIO_u8PORTB,DIO_u8PIN3,DIO_u8PIN_LOW);

	u8 Local_au8Password[4];
	u8 Local_u8Flag = 0;
	u8 Local_u8ReadPassword = KPD_NO_PRESSED_KEY;
	u8 Local_u8Tries = Max_Tries;
	u8 i,j;

	if(EEPROM_u8ReadeData(EEPROM_Status_Flag)==First_Use) // to check if its the first time to use the safe
	{
		CLCD_voidSendString("Set Password:");
		for( i=0; i<=3 ; i++)
		{
			do
			{
				Local_u8ReadPassword = KPD_u8GetPressedKey();

			} while (Local_u8ReadPassword == KPD_NO_PRESSED_KEY);

			CLCD_voidGoToXY(1,i);
			CLCD_voidSendData(Local_u8ReadPassword);
			_delay_ms(500);
			CLCD_voidGoToXY(1,i);
			CLCD_voidSendData('*');
			EEPROM_enuWriteData(EEPROM_PASSWORD_LOCATION1+i,Local_u8ReadPassword);
		}

		EEPROM_enuWriteData(EEPROM_Status_Flag,Entered_The_Safe); // set the flag to 0
	}

	for(;;)
	{
		while(Local_u8Flag == 0)
		{
			CLCD_voidClearScreen();

			Local_u8ReadPassword = KPD_NO_PRESSED_KEY;
			CLCD_voidSendString("1-Enter Pass");
			CLCD_voidGoToXY(1,0);
			CLCD_voidSendString("2-Change Pass");
			do
			{
				Local_u8ReadPassword = KPD_u8GetPressedKey();

			} while (Local_u8ReadPassword == KPD_NO_PRESSED_KEY);
			_delay_ms(240);
			if(Local_u8ReadPassword == '1')
			{
				CLCD_voidClearScreen();
				CLCD_voidSendString("Enter Password:");
				for( i =0 ; i<=3 ; i++)
				{
					do
					{
						Local_au8Password[i] = KPD_u8GetPressedKey();
					} while (Local_au8Password[i]==KPD_NO_PRESSED_KEY);

					CLCD_voidGoToXY(1,i);
					CLCD_voidSendData(Local_au8Password[i]);
					_delay_ms(500);
					CLCD_voidGoToXY(1,i);
					CLCD_voidSendData('*');
				}

				if((EEPROM_u8ReadeData(EEPROM_PASSWORD_LOCATION1)==Local_au8Password[0]) &&  (EEPROM_u8ReadeData(EEPROM_PASSWORD_LOCATION2)==Local_au8Password[1]) && (EEPROM_u8ReadeData(EEPROM_PASSWORD_LOCATION3)==Local_au8Password[2]) && (EEPROM_u8ReadeData(EEPROM_PASSWORD_LOCATION4)==Local_au8Password[3]) )
				{
					CLCD_voidClearScreen();
					CLCD_voidSendString("Right Password");
					CLCD_voidGoToXY(1,0);
					CLCD_voidSendString("Safe Opened");
					DIO_u8SetPinValue(DIO_u8PORTB,DIO_u8PIN0,DIO_u8PIN_HIGH);
					DIO_u8SetPinValue(DIO_u8PORTB,DIO_u8PIN1,DIO_u8PIN_LOW);
					for(j=0;j<100;j++)
					{
						SERVO_voidTimer1ServoSetAngleOCR1A(j);
						SERVO_voidTimer1ServoSetAngleOCR1B(j);
						_delay_ms(15);
					}
					_delay_ms(5000);
					for(j=100;j>0;j--)
					{
						SERVO_voidTimer1ServoSetAngleOCR1A(j);
						SERVO_voidTimer1ServoSetAngleOCR1B(j);
						_delay_ms(15);
					}
					DIO_u8SetPinValue(DIO_u8PORTB,DIO_u8PIN0,DIO_u8PIN_LOW);
					DIO_u8SetPinValue(DIO_u8PORTB,DIO_u8PIN1,DIO_u8PIN_HIGH);

				}
				else if((EEPROM_u8ReadeData(EEPROM_PASSWORD_LOCATION1)==Local_au8Password[3]) &&  (EEPROM_u8ReadeData(EEPROM_PASSWORD_LOCATION2)==Local_au8Password[2]) && (EEPROM_u8ReadeData(EEPROM_PASSWORD_LOCATION3)==Local_au8Password[1]) && (EEPROM_u8ReadeData(EEPROM_PASSWORD_LOCATION4)==Local_au8Password[0]) )
				{
					CLCD_voidClearScreen();
					CLCD_voidGoToXY(0,1);
					CLCD_voidSendString("Calling Police");
					DIO_u8SetPinValue(DIO_u8PORTB,DIO_u8PIN1,DIO_u8PIN_LOW);
					DIO_u8SetPinValue(DIO_u8PORTB,DIO_u8PIN2,DIO_u8PIN_HIGH);
					_delay_ms(5000);
					DIO_u8SetPinValue(DIO_u8PORTB,DIO_u8PIN1,DIO_u8PIN_HIGH);
					DIO_u8SetPinValue(DIO_u8PORTB,DIO_u8PIN2,DIO_u8PIN_LOW);
					Local_u8Flag=0;
				}
				else
				{
					Local_u8Tries--;
					/* Check if the three tries are */
						if(Local_u8Tries>0)
						{
							CLCD_voidClearScreen();
							CLCD_voidSendString("Wrong Password");
							_delay_ms(1000);
							CLCD_voidClearScreen();
							CLCD_voidSendString("Tries Left:");
							CLCD_voidSendData(Local_u8Tries+48);
							_delay_ms(1000);
						}
						else
						{
							CLCD_voidClearScreen();
							CLCD_voidSendString("Wrong Password");
							CLCD_voidGoToXY(1,0);
							CLCD_voidSendString("Safe Closed 30s");
							DIO_u8SetPinValue(DIO_u8PORTB,DIO_u8PIN3,DIO_u8PIN_HIGH);
							_delay_ms(30000);
							DIO_u8SetPinValue(DIO_u8PORTB,DIO_u8PIN3,DIO_u8PIN_LOW);
							Local_u8Tries = 3;
							Local_u8Flag=0;
							continue;
						}
					}
				}
				else if(Local_u8ReadPassword == '2')
				{
					CLCD_voidClearScreen();
					CLCD_voidSendString("old Password:");
					for( i =0 ; i<=3 ; i++)
					{
						do
						{
							Local_au8Password[i] = KPD_u8GetPressedKey();
						} while (Local_au8Password[i]==KPD_NO_PRESSED_KEY);

						CLCD_voidGoToXY(1,i);
						CLCD_voidSendData(Local_au8Password[i]);
						_delay_ms(500);
						CLCD_voidGoToXY(1,i);
						CLCD_voidSendData('*');
					}


					if((EEPROM_u8ReadeData(EEPROM_PASSWORD_LOCATION1)==Local_au8Password[0]) &&  (EEPROM_u8ReadeData(EEPROM_PASSWORD_LOCATION2)==Local_au8Password[1]) && (EEPROM_u8ReadeData(EEPROM_PASSWORD_LOCATION3)==Local_au8Password[2]) && (EEPROM_u8ReadeData(EEPROM_PASSWORD_LOCATION4)==Local_au8Password[3]) )
					{
						CLCD_voidClearScreen();
						CLCD_voidSendString("New Password:");
						for( i =0; i<=3 ; i++)
						{

							do
							{
								Local_u8ReadPassword = KPD_u8GetPressedKey();

							} while (Local_u8ReadPassword == KPD_NO_PRESSED_KEY);

							CLCD_voidGoToXY(1,i);
							CLCD_voidSendData(Local_u8ReadPassword);
							_delay_ms(500);
							CLCD_voidGoToXY(1,i);
							CLCD_voidSendData('*');
							EEPROM_enuWriteData(EEPROM_PASSWORD_LOCATION1+i,Local_u8ReadPassword);
						}
					}
					else
					{

						CLCD_voidClearScreen();
						CLCD_voidSendString("Wrong Password");
						_delay_ms(1000);
					}
				}
				else
				{
					CLCD_voidClearScreen();
					CLCD_voidSendString("Wrong Choice");
					_delay_ms(1000);
				}

			}

		}
	}
