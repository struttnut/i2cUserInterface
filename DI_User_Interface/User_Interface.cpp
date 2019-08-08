/*
LCD_Menu.cpp - Library for menu system creation for use with I2C Keypad and I2C LCD.
Created by David Strutt, May 2019.
Released into the public domain.

depending on uses, might need to create our own string to float function, since it only returns 2 decimal places
might be ok, if we use the floats in function, and only use strings for display, exception for very small values.
*/

#include "User_Interface.h"

User_Interface::init(int lcd_address, int kpd_address)
{
	lcd.init(lcd_address,20,4);
	lcd.backlight();
	
	kpd.init(kpd_address, 4, 4);

}

char User_Interface::key_input() //returns single key input from keypad - blocking
{
	char key;
	do{
		key = kpd.get_key();
	}while(key == '\0');
	return key;
}

//next two functions should be combined into template form?.
void User_Interface::disp(int row, char pos, String toBeDisplayed) //adds the 'toBeDisplayed' String to the 'row' (starting with 1) at the 'pos' (where L/l:for left side column, C/c:for centered column, R/r:for right side column)
{
	int from_left;

	switch(pos)
	{
	case 'L': case 'l':
		from_left = 0;
		break;
	case 'C': case 'c':
		from_left=(20-toBeDisplayed.length())/2;  //  [(total width of display - 20 characters) - (length of string)] / 2
		break;
	case 'R': case 'r':
		from_left = (20-toBeDisplayed.length()); // (total width of display - 20 characters) - (length of string)
		break;
	}
	lcd.setCursor(from_left,row - 1);
	lcd.print(toBeDisplayed);
}

void User_Interface::disp(int row, char pos, float toBeDisplayed)//adds the 'toBeDisplayed' String to the 'row' (starting with 1) at the 'pos' (where L/l:for left side column, C/c:for centered column, R/r:for right side column)
{
	int from_left;

	switch(pos)
	{
	case 'L': case 'l':
		from_left = 0;
		break;
	case 'C': case 'c':
		from_left=(20-String(toBeDisplayed).length())/2;  //  [(total width of display - 20 characters) - (length of string)] / 2
		break;
	case 'R': case 'r':
		from_left = (20-String(toBeDisplayed).length()); // (total width of display - 20 characters) - (length of string)
		break;
	}
	lcd.setCursor(from_left,row - 1);
	lcd.print(toBeDisplayed);
}

//next two functions should be combined into template form?.
void User_Interface::disp_clear(int row, char pos, String toBeCleared) //clears the LCD at row,pos (per 'disp' function) for a number of positions equal to the length of the 'toBeCleared'
{
	String temp_blanks;
	for (int i=1;i<=toBeCleared.length();i++)
	{
		temp_blanks = temp_blanks + " ";
	}
	this->disp(row,pos,temp_blanks);
	
}

void User_Interface::disp_clear(int row, char pos, float toBeCleared) //clears the LCD at row,pos (per 'disp' function) for a number of positions equal to the length of the 'toBeCleared'
{
	String temp_blanks;
	for (int i=1;i<=String(toBeCleared).length();i++)
	{
		temp_blanks = temp_blanks + " ";
	}
	this->disp(row,pos,temp_blanks);
	
}

float User_Interface::inputMenu(String title, float cur_val = 0) //returns float value entered from interface
{
	String temp = String(cur_val); //send default float value to a string value for display

	bool default_change = true; //flag for clearing default display - screen will show current/default value before entry begins
	
		//set up lcd display
		lcd.clear();
		this->disp(1,'L',title);
		this->disp(2,'C',temp);
		this->disp(4,'L',"*:DEL"); this->disp(4,'C',"#:."); this->disp(3,'R',"C:+/-"); this->disp(4,'R',"D:ENT");


	//get a keypress from the keypad then intrepret.
	char key;
	do{
		key = this->key_input();
		if (default_change == true) { this->disp_clear(2,'C',temp); temp = ""; default_change = false;} //if first key stroke, clear screen of current/default value
		switch (key)
		{
		case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
			this->disp_clear(2,'C',temp); //clears current displayed value
			temp = temp + key;
			this->disp(2,'C',temp); //displays new value
			break;
		case '*':
			this->disp_clear(2,'C',temp); //clears current displayed value
			temp = temp.substring(0,temp.length()-1);
			this->disp(2,'C',temp);//displays new value
			break;
		case '#':
			this->disp_clear(2,'C',temp); //clears current displayed value
			if (temp == ""){temp = "0.";}
			else{	temp = temp + ".";}
			this->disp(2,'C',temp); //displays new value		
			break;
		case 'C':
			this->disp_clear(2,'C',temp); //clears current displayed value
			if (temp.substring(0,1) == "-"){temp = temp.substring(1,temp.length());}
			else {temp = "-" + temp;}
			this->disp(2,'C',temp); //displays new value		
			break;
		} 
		
	}while (key != 'D'); //'D' is 'Enter' and represents completion of value entry

	//convert the string to number and return it
	return temp.toFloat();
	
}

float User_Interface::settingMenu(char **names, float values[], int size, String title = "Setting:")//updates an array of "settings" - menu interface will loop through the 'names' while displaying the current 'values' - the 'names' and 'settings' arrays are of size 'size'
{

	String temp; 
	if(active_menu != "settingMenu")
	{
		active_menu = "settingMenu";
		//set up lcd display
		lcd.clear();
		this->disp(1,'L',title);
		this->disp(2,'L',names[0]);
		this->disp(3,'L',values[0]);
		this->disp(1,'R',"Exit:A"); this->disp(2,'R',"Edit:B"); this->disp(3,'R',"Prev:C"); this->disp(4,'R',"Next:D");
	}	
	int i=0;
	char key;

	do{
		key = this->key_input();

		switch (key)
		{
		case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
		case 'B':
			temp = names[i];
			lcd.clear();
			values[i] = this->inputMenu("Enter " + temp + ":", values[i]); //enter a new value for the value.
			lcd.clear();
			this->disp(1,'L',"Setting:");
			this->disp(2,'L',names[i]);
			this->disp(3,'L',values[i]);
			this->disp(1,'R',"Exit:A"); this->disp(2,'R',"Change:B"); this->disp(3,'R',"Previous:C"); this->disp(4,'R',"Next:D");
			break;
		case 'C': //Previous
			this->disp_clear(2,'L',names[i]);
			this->disp_clear(3,'L',values[i]);
			if (i==0){i = size-1;}
			else {i--;}
			this->disp(2,'L',names[i]);
			this->disp(3,'L',values[i]);
			break;
		case 'D': //Next
			this->disp_clear(2,'L',names[i]);
			this->disp_clear(3,'L',values[i]);
			if (i==size-1){i = 0;}
			else {i++;}
			this->disp(2,'L',names[i]);
			this->disp(3,'L',values[i]);
			break;
		}
		
	}while (key != 'A'); //'A' is change key which triggers 'change' to inputMenu

}

int User_Interface::EEPROM_Save_Settings(float values[], int size, int start=0)//store "settings" to EEPROM, where "size" is the number of itmes in the "values" arrays, and "start" is the starting eeprom address. The next available eeprom address is returned.
{
	//promt to save settings to EEPROM, otherwise change to settings will be lost on power-off_type
	lcd.clear();
	this->disp(1,'L',"Save Settings to");
	this->disp(2,'L',"EEPROM?");
	this->disp(3,'R',"NO:C");
	this->disp(4,'R',"YES:D");
	char key;
	int eeAddress = start; //EEPROM address to start writing to
	do{
		key = this->key_input();
		if (key =='D')
		{
			
			for (int i = 0; i<= (size-1); i++)
			{
				EEPROM.put(eeAddress, values[i]);
				eeAddress += sizeof(values[i]);
			}
			
		}
		
	}while (key!='C' && key!='D');
	
	lcd.clear();	
	
	return eeAddress;
}

void User_Interface::EEPROM_Read_Settings(float values[], int size, int start = 0) //read previously stored "settings" from EEPROM where "size" is the number of itmes in the "values" arrays, and "start" is the starting eeprom address.
{
	int eeAddress = start; //EEPROM address to start reading from
	for (int i = 0; i<= (size-1); i++)
	{
		EEPROM.get(eeAddress, values[i]);
		eeAddress += sizeof(values[i]);
	}
}

String User_Interface::modeMenu(char **names, int size, String title1 = "Select", String title2 = "Mode:") //display blocking mode menu, and return selected mode, or extened press key character, blocks until key is input.
{

	char key;
	int i=0;
	if(active_menu != "modeMenu")
	{
		active_menu = "modeMenu";
	}
 do
 {
		lcd.clear();
		this->disp(1,'L',title1);
		this->disp(2,'L',title2);
		this->disp(1,'R',String(names[i]) + ":A");
		if(i+1 <= size-1){this->disp(2,'R',String(names[i+1]) + ":B");}
		if(i+2 <= size-1){this->disp(3,'R',String(names[i+2]) + ":C");}
		if(size >4){this->disp(4,'R',"More...:D");}
		else if(size==4){this->disp(4,'R',String(names[i+3]) + ":D");}

		//need to capture keystrokes, and return the mode string
		key = this->key_input();
		
		switch(key)
		{
		case 'A':
			return names[i];
			break;
		case 'B':
			if (i+1 < size){return names[i+1];}
			break;
		case 'C':
			if (i+2 < size){return names[i+2];}
			break;
		case 'D':
 			if (size ==4){return names[3];}
			else if (size < 4){}
			else if (i+3 >= size){i = 0;}
			else {i=i+3;}
      break;
		default:
			if (key >= 69 && key <= 84)
			{
				this->lcd.clear();
				return String(key);
			}
			break;
		}

	}while(1);
}

void User_Interface::infoScreen(String information) //display information to user, wait for confirmation
{
	int end_of_line;
	
	lcd.clear();
	
	if (information.length() > 20){end_of_line = information.lastIndexOf(" ",20);}
	else {end_of_line = information.length();}
	this->disp(1,'L',information.substring(0,end_of_line));
	String remaining_information = information.substring(end_of_line+1);
	
	if (remaining_information.length() > 20){end_of_line = remaining_information.lastIndexOf(" ",20);}
	else {end_of_line = remaining_information.length();}
	this->disp(2,'L',remaining_information.substring(0,end_of_line));
	remaining_information = remaining_information.substring(end_of_line+1);	
	
	if (remaining_information.length() > 20){end_of_line = remaining_information.lastIndexOf(" ",20);}
	else {end_of_line = remaining_information.length();}
	this->disp(3,'L',remaining_information.substring(0,end_of_line));
	remaining_information = remaining_information.substring(end_of_line+1);
	
	if (remaining_information.length() > 20){end_of_line = remaining_information.lastIndexOf(" ",20);}
	else {end_of_line = remaining_information.length();}
	this->disp(4,'L',remaining_information.substring(0,end_of_line));
	remaining_information = remaining_information.substring(end_of_line+1);
	
	this->disp(4,'R'," OK:D");
	do
	{
	} while (this->key_input() != 'D');

}

void User_Interface::statusScreen(char **titles, float values[], int size)//display status of values, refreshed at given 'refresh'.
{
	if(active_menu != "statusMenu")
	{
		active_menu = "statusMenu";
		this->lcd.clear();
		this->disp(4,'R',"Exit:D");
		for (int i=0; i<=size-1; i++)
		{
			this->disp(i+1,'L',titles[i]);
			this->disp(i+1,'C',values[i]);
			current_stats[i] = values[i];
		}
		
	}
	for (int i=0; i<=size-1; i++)
	{
		if (current_stats[i] != values[i] && millis() > (last_refresh + refresh_rate))
		{
			last_refresh = millis();
			this->disp_clear(i+1,'C',String(values[i])+"  "); //add space to ensure previous value is cleared
			this->disp(i+1,'C',values[i]);
			current_stats[i] = values[i];
		}		

	}
	
}


