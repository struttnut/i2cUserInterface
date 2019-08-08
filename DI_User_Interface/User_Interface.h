/*
LCD_Menu.h - Library for menu system creation for use with I2C Keypad and I2C LCD.
Created by David Strutt, May 2019.
Released into the public domain.
*/

#ifndef LCD_Menu_h
#define LCD_Menu_h

#include "Arduino.h"
#include "LiquidCrystal_I2C.h"
#include "i2ckeypad.h"
#include "EEPROM.h"

class User_Interface
{
public:
	
	LiquidCrystal_I2C lcd;
	i2ckeypad kpd;
	init(int lcd_address, int kpd_address);
	char key_input(); //returns single key input from keypad
	void disp_clear(int row, char pos, String toBeCleared); //clears the LCD at row,pos (per 'disp' function) for a number of positions equal to the length of the 'toBeCleared'
	void disp(int row, char pos, String toBeDisplayed); //adds the 'toBeDisplayed' String to the 'row' (starting with 1) at the 'pos' (where L/l:for left side column, C/c:for centered column, R/r:for right side column)
	void disp_clear(int row, char pos, float toBeCleared); //overloaded for displaying float values
	void disp(int row, char pos, float toBeDisplayed); //overloaded for displaying float values
	float inputMenu(String title, float cur_val = 0); //returns float value entered from interface
	float settingMenu(char **names, float values[], int size, String title = "Setting:"); //updates an array of "settings" - menu interface will loop through the 'names' while displaying the current 'values' - the 'names' and 'settings' arrays are of size 'size' - "title" can be changed from default if needed.
	void EEPROM_Read_Settings(float values[], int size, int start = 0); //read previously stored "settings" from EEPROM where "size" is the number of itmes in the "values" arrays, and "start" is the starting eeprom address.
	int EEPROM_Save_Settings(float values[], int size, int start=0); //store "settings" to EEPROM, where "size" is the number of itmes in the "values" arrays, and "start" is the starting eeprom address. The next available eeprom address is returned.
	String modeMenu(char **names, int size, String title1 = "Select", String title2 = "Mode:"); //display blocking mode menu, and return selected mode, or extened press key character, blocks until key is input. - "title1" and "title2" can be changed from default as needed.
	void infoScreen(String information); //display information to user, wait for confirmation
	void statusScreen(char **titles, float values[], int size);//display status of values, refreshed at given 'refresh'.

	String active_menu = ""; //stores currently displayed menu, to avoid unnessecary screen updates for certain applications
	float current_stats[3]; //stores currently displayed stats, to avoid unnessecary screen updates for certain applications
	unsigned long last_refresh=0;
	unsigned long refresh_rate = 1000;
};//end of UI Class

#endif

