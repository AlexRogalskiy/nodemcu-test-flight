
/* DIGISPARK GSM SWITCH

     This program uses a Digispark to remotely, via GSM, do one function (turn on or off one relay, for instance);
     In this case it simply toggles the state of the onboard LED, and sends the current state via sms to predefined recipients.

     It can definitely be improved to work remotely with more channels/relays and/or using other pins, but right now, I'm doing my best with
     the knowledge I have.


*/



#include <SoftSerial.h>

char phone_no1[] = "xxxxxxxxxx";
/* Uncomment following line if you want more than one phone number to be texted.
char phone_no2[] = "yyyyyyyyyy";
*/

boolean toggle = false;
SoftSerial A6(2, 3); //rx,tx

void setup()
{

  pinMode (0, OUTPUT);
  digitalWrite (0, HIGH); // initial relay setting: OFF
  delay (1000); //letting things settle down on the modem side.

  A6.begin(9600); // open serial comm.

  //Start sending something out the serial connection to force autobaud to 9600 bps

  for (int a = 0; a < 50; a++)
  {
    A6.println("AT");
    delay(200);
  }

  delay(2000);

  A6.flush(); //clearing serial buffer.
  delay(2000);

}



void loop()
{
  //keeps checking for any sort of incoming data the gsm module sends whenever there is activity,
  //either due to an sms or phone call.

  if (A6.available())
  {
    toggle = !toggle; //toggles the previous state of the variable. On to Off, or Off to On.
    digitalWrite (1, toggle); // writes the state to pin 1.
    sms(); // calls the function that sends the current state via sms.
    delay(10000); //delay needed to avoid extra incoming data from reversing the state of "toggle".
    A6.flush(); // really needed, otherwise A6.available() is always true due to having data on the buffer.
  }

}


void sms() //function that sends the state of the variable "toggle" via sms to the main phone number
{
  delay(8000); //letting it ring just a bit, in case it's a phonecall
  A6.println("ATH"); // hangs up incoming call
  delay(2000); // delay needed before next command.

  // Sending to phone number 1
  A6.println("AT+CMGF=1"); // setting sms mode
  delay(2000);
  A6.print("AT+CMGS=\"");
  A6.print(phone_no1);
  A6.write(0x22);
  A6.write(0x0D);  // hex equivalent of Carraige return
  A6.write(0x0A);  // hex equivalent of newline
  delay(2000);

  if (toggle == 1)
  {
    A6.print("On");
  }
  else
  {
    A6.print("Off");
  }
  delay(500);
  A6.println (char(26));//the ASCII code of the ctrl+z is 26
  delay(2000);

  // Sending to phone number 2 
  
  /* Uncomment following lines if you want more than one phone number to be texted, by removing "/*" and "* /"
   
  A6.println("AT+CMGF=1"); // setting sms mode
  delay(2000);
  A6.print("AT+CMGS=\"");
  A6.print(phone_no2);
  A6.write(0x22);
  A6.write(0x0D);  // hex equivalent of Carraige return
  A6.write(0x0A);  // hex equivalent of newline
  delay(2000);


  if (toggle == 1)
  {
    A6.print("On");
  }
  else
  {
    A6.print("Off");
  }
  delay(500);
  A6.println (char(26));//the ASCII code of the ctrl+z is 26

*/

  // toggling relay.
  //won't work within the "if" statements above, so needs another just to toggle the relay state.

  if (toggle == 1)
  {
    digitalWrite (0, LOW);
  }
  else
  {
    digitalWrite (0, HIGH);
  }
}




