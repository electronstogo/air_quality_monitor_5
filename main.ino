#include "bme280.h"
#include <Wire.h>
#include "ssd1306.h"
#include <stdio.h>
#include "sgp40.h"





void setup()
{
	// init arduino i2c framework class.
	Wire.begin();
}




void loop()
{
	// some utility helping variables.
	char char_buffer[20];
	String string_buffer;
	
	// timestamp to handle regular 1Hz measurements with SGP40 sensor.
	unsigned long sgp40_timestamp;


	// SGP40 sensor class variable to read SGP40 measurements.
	SGP40Sensor sgp40_sensor;

	// Initiate variables of BME280 sensor class and for measurments.
	BME280Sensor bme280_sensor;
	int32_t temperature;
	uint32_t pressure, humidity;

	// Initiate a SSD1306 OLED display class variable.
	SSD1306 ssd1306(0x3C);


  

	while(true)
	{
		// clear OLED display memory.
		ssd1306.clear_oled_buffer();
		
		
		// print measured VOC index on the OLED display.
		string_buffer = "VOC: ";

		// get the VOC index from SGP40.
		unsigned int voc_index;
		if(!sgp40_sensor.get_voc_index((float)temperature / 100.0, (float)humidity / 1000.0, &voc_index))
		{
			// error case, sth. wrent wrong with SGP40 sensor.
			string_buffer += "ERROR";
		}
		else
		{
			// add the determined VOC index to string.
			string_buffer += String(voc_index);
		}

		// remember the time after the SGP40 measurement was done.
		sgp40_timestamp = millis();
		
		
		string_buffer.toCharArray(char_buffer, 20);	
		ssd1306.draw_string(0, 55, char_buffer);


		// get measurements from BME280 sensor.
		bme280_sensor.do_humidity_temperature_pressure_measurement(&temperature, &pressure, &humidity);


		// print the pressure measurement.
		string_buffer = String(pressure);
		string_buffer += " MBAR";
		string_buffer.toCharArray(char_buffer, 20);
		ssd1306.draw_string(0, 35, char_buffer);

		// print humidity & temperature measurements.
		string_buffer = String((float)humidity / 1000.0, 1);
		string_buffer += "% ";
		string_buffer += String((float)temperature / 100.0, 1);
		// the $ character is a workaround to get a ° character in the SSD1306 library.
		string_buffer += "$C"; 
		string_buffer.toCharArray(char_buffer, 20);
		ssd1306.draw_string(0, 15, char_buffer);


		// flush current OLED memory content to the display.
		ssd1306.flush_oled_buffer();



		// handle the necessary delay for the SGP40 sensor.
		// check the time left since a SGP40 measurement was done. 
		unsigned int calculated_delay = 1000 - (millis() - sgp40_timestamp); 
		
		// do a delay for the remaining time.
		if(calculated_delay < 1000)
		{
			delay(calculated_delay);
		}
	}
}
