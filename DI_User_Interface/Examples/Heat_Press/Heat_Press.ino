#include"User_Interface.h"

//TODO
//work out saving of settings -done 6/17/2019
//issue with status displayed some of previous value...was 41, changed to 2, displayed 42 first time. -maybe done 6/17/2019, fixed not sure if best way - added spaces to disp_clear function, but don't see why this was necessary
//default menu displays - allow custom - overload the functions? -done 6/17/2019
//do a example that is a pin controller - allows to read input, set outputs, pwms etc. make a multipurpose controller. like a function generator?

User_Interface UI;

const int presspin = 2; //digital output pin that initiates press
const int heatpin = 3; //digital output pin that turns on heat
const int temppin = A0; //analog input pin measures temperature

//The following settings are more commonly used, and readily available to the user.
char *setting_names[] = {"Time", "Temp"};
float setting_vals[] = {70, 180};
const int num_settings = sizeof(setting_vals)/sizeof(setting_vals[0]); //automatically determines size of array, no need to adjust

//The following settings are more rarely used, and setup type of settings. They are less readily available (require an unlabeled-long key press to access)
char *deep_setting_names[] = {"P", "I", "D", "Heat Corr.", "Time def.", "Temp def."};
float deep_setting_vals[] = {1.0, 2.0, 3.0, 0.0, 70, 180};
const int num_deep_settings = sizeof(deep_setting_vals)/sizeof(deep_setting_vals[0]); //automatically determines size of array, no need to adjust

//These are the main modes 
char *mode_names[] = {"Time/Temp","Status","","Press"};
const int num_modes = sizeof(mode_names)/sizeof(mode_names[0]); //automatically determines size of array, no need to adjust

//These are the stats to be displayed on a status screen - note maximum of 3 stats, if more are needed, make additional sets of arrays and calls to the function.
char *status_names[] = {"Time", "Temp"};
float status_vals[] = {1.0, 2.0};
const int num_stats = sizeof(status_vals)/sizeof(status_vals[0]); //automatically determines size of array, no need to adjust

String main_mode_selection;

#define time_setting setting_vals[0]
#define temp_setting setting_vals[1]
float start_time;


void setup() {
	// put your setup code here, to run once:

	UI.init(0x27,0x20); //LCD i2c address, Keypad i2c address

  UI.EEPROM_Read_Settings(deep_setting_vals,num_deep_settings);
  time_setting = deep_setting_vals[4];
  temp_setting = deep_setting_vals[5];

	pinMode(presspin, OUTPUT);
	pinMode(heatpin, OUTPUT);
	pinMode(temppin, INPUT);

	//UI.statusScreen(status_names,status_vals,num_stats);
	//this only partly works, we need to pul out the keypress watcher, and continually call the above function
	//UI.lcd.clear();
	//UI.show();

	//UI.disp(4,1,String(UI.inputMenu("enter some",756.567)));
	//UI.EEPROM_Read_Settings(setting_vals,num_settings);
	//UI.settingMenu(setting_names,setting_vals,num_settings);
	//delay(500);

	//UI.settingMenu(setting_names,setting_vals,num_settings);

	//UI.modeMenu("Select Mode","da","te","la","pa");

	//UI.disp(1,'L',"Custom something");

	//String temp = UI.modeMenu(mode_names,num_modes);
	//UI.disp(1,'L',temp);
  //delay(2000);
	//UI.infoScreen("This is the information screen test to see how well the lines wrap");
	//UI.infoScreen("balh");
	//UI.lcd.clear();
}

void loop() {
	// put your main code here, to run repeatedly:

	main_mode_selection = UI.modeMenu(mode_names,num_modes);

	if (main_mode_selection == "Time/Temp") 
	{
		UI.settingMenu(setting_names,setting_vals,num_settings); //change the settings
    //no option to save these to eeprom, the defaults from the deep settings menu will be used.
  }
	else if (main_mode_selection == "Status") 
	{
		do
		{
			UI.statusScreen(status_names,status_vals,num_stats);
			//update time and temp values at some interval
		}while(UI.kpd.get_key() != 'D');
	}
	else if (main_mode_selection == "Press") // "Press"
	{
		start_time = millis();

		digitalWrite(presspin, HIGH); //press down

		UI.statusScreen(status_names,status_vals,num_stats);
		
		//show a up option - overwrites the status menu "exit" option
		UI.disp_clear(4,'R',"Exit:D");
		UI.disp(4,'R',"UP:D");

		do
		{
			UI.statusScreen(status_names,status_vals,num_stats);//updates the status
			status_vals[0] = round(time_setting - (millis()-start_time)/1000); //update time value
		}while((millis() < start_time + time_setting*1000) && (UI.kpd.get_key() != 'D'));
		

		digitalWrite(presspin, LOW); // press up

	}
	else if(main_mode_selection == "E") //long press of 1 key
	{
    UI.settingMenu(deep_setting_names,deep_setting_vals,num_deep_settings);      
    UI.EEPROM_Save_Settings(deep_setting_vals,num_deep_settings);
	}
}



