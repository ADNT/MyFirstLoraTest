#include <WiFi.h>
#include <U8x8lib.h>
#include <LoRa.h>

// the OLED used
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

#define SS      18
#define RST     14
#define DI0     26

void setup()
{
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  delay(100);

  u8x8.begin();
  u8x8.setFont(u8x8_font_5x8_f);
  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(SS, RST, DI0); 
  if (!LoRa.begin(868E6)) {
	  u8x8.drawString(0, 1, "Error with LoRa");
    while (1);
  }
  u8x8.drawString(0, 1,   "LoRa Configured");
}


static void doSomeWork()
{
	const int n =  WiFi.scanNetworks();
	for (int i = 0; i < n; ++i) {
	  // Print SSID for each network found
	  char currentSSID[64];
	  WiFi.SSID(i).toCharArray(currentSSID, 64);
	  u8x8.drawString(0, i + 1, currentSSID);
	}
  // Wait a bit before scanning again
  delay(5000);
}


void loop()
{
  doSomeWork();
}
