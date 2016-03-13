
#include <Wire.h>
#include <ADXL345.h>

#include <b64.h>
#include <HttpClient.h>


#include <LDateTime.h>
#include <LTask.h>
#include <LWiFi.h>
#include <LWiFiClient.h>

#define WIFI_AP "Mi"
#define WIFI_PASSWORD "123456789"
#define WIFI_AUTH LWIFI_WPA  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP.
#define per 50
#define per1 3
#define DEVICEID "DRlWx0Mp" // Input your deviceId
#define DEVICEKEY_TEMP "FySyiDHptBS6rTCN" // Input your deviceKey
#define DEVICEKEY_RH "FySyiDHptBS6rTCN"
#define SITE_URL "api.mediatek.com"


LWiFiClient c;
unsigned int rtc;
unsigned int lrtc;
unsigned int rtc1;
unsigned int lrtc1;
char port[4]={0};
char connection_info[21]={0};
char ip[21]={0};             
int portnum;
int val = 0;
String tcpdata = String(DEVICEID) + "," + String(DEVICEKEY_TEMP) + ",0";
String upload_data;

//int Moisture = 0;
int p1=0,p2=0,p3=0,p4=0,p5=0;

datetimeInfo t;

LWiFiClient c2;
HttpClient http(c2);

 //p1=0;
ADXL345 adxl; //variable adxl is an instance of the ADXL345 library

void setup(){
  
  adxl.powerOn();

  //set activity/ inactivity thresholds (0-255)
  adxl.setActivityThreshold(75); //62.5mg per increment
  adxl.setInactivityThreshold(75); //62.5mg per increment
  adxl.setTimeInactivity(10); // how many seconds of no activity is inactive?
 
  //look of activity movement on this axes - 1 == on; 0 == off 
  adxl.setActivityX(1);
  adxl.setActivityY(1);
  adxl.setActivityZ(1);
 
  //look of inactivity movement on this axes - 1 == on; 0 == off
  adxl.setInactivityX(1);
  adxl.setInactivityY(1);
  adxl.setInactivityZ(1);
 
  //look of tap movement on this axes - 1 == on; 0 == off
  adxl.setTapDetectionOnX(0);
  adxl.setTapDetectionOnY(0);
  adxl.setTapDetectionOnZ(1);
 
  //set values for what is a tap, and what is a double tap (0-255)
  adxl.setTapThreshold(50); //62.5mg per increment
  adxl.setTapDuration(15); //625us per increment
  adxl.setDoubleTapLatency(80); //1.25ms per increment
  adxl.setDoubleTapWindow(200); //1.25ms per increment
 
  //set values for what is considered freefall (0-255)
  adxl.setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment
 
  //setting all interrupts to take place on int pin 1
  //I had issues with int pin 2, was unable to reset it
  adxl.setInterruptMapping( ADXL345_INT_SINGLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_DOUBLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_FREE_FALL_BIT,    ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_ACTIVITY_BIT,     ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_INACTIVITY_BIT,   ADXL345_INT1_PIN );
 
  //register interrupt actions - 1 == on; 0 == off  
  adxl.setInterrupt( ADXL345_INT_SINGLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_DOUBLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_FREE_FALL_BIT,  1);
  adxl.setInterrupt( ADXL345_INT_ACTIVITY_BIT,   1);
  adxl.setInterrupt( ADXL345_INT_INACTIVITY_BIT, 1);


  //we have added new code setup method for wifi conncetivity 
   LTask.begin();
   LWiFi.begin();
// Serial.begin(115200);
  Serial.begin(9600);
  while(!Serial) delay(1000); /* comment out this line when Serial is not present, ie. run this demo without connect to PC */
  Serial.println("Setup..");

  Serial.println("Connecting to AP");
  while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD)))
  {
    delay(1000);
  }
  Serial.println("calling connection");

  while (!c2.connect(SITE_URL, 80))
  {
    Serial.println("Re-Connecting to WebSite");
    delay(1000);
  }
  delay(100);

  getconnectInfo();
  connectTCP();
}

void loop(){
  
	//Boring accelerometer stuff   
	int x,y,z;  
	adxl.readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z
	// Output x,y,z values 
	Serial.print("values of X , Y , Z: ");
	Serial.print(x);
	Serial.print(" , ");
	Serial.print(y);
	Serial.print(" , ");
	Serial.println(z);
	
	double xyz[3];
	double ax,ay,az;
	adxl.getAcceleration(xyz);
	ax = xyz[0];
	ay = xyz[1];
	az = xyz[2];
  
	Serial.print("X=");
	Serial.print(ax);
    Serial.println(" g");
	Serial.print("Y=");
	Serial.print(ay);
    Serial.println(" g");
	Serial.print("Z=");
	Serial.println(az);
    Serial.println(" g");
	Serial.println("**********************");
	delay(50);
if (0<ax<0.35 && 0<ay<0.3 && 0.4<az<0.7 && p1==0 )
{
  p1++; 
}
if (235<ax<255 && 0.25<ay<0.45 && 0.2<az<0.5  && p1==1)
{
  p1++; 
}
if (240<ax<250 && 0<ay<0.3 && 0<az<0.7 && p1==2)
{
  p1++; 
}
if (230<ax<250 && 0<ay<0.1 && 0.5<az<0.75  && p1==3)
{
  p1++; 
}
if (240<ax<260 && 0<ay<0.15 && 0<az<0.8 && p1==4)
{
  p1++; 
}

if (0.6<ax<0.8 && 0<ay<0.3 && 210<az<235 && p1==5)
{
  p1++; 
}
if (0.7<ax<1 && 0<ay<0.2 && 0<az<0.35 && p1==6)
{
  p1++; 
}



if (0<ax<0.2 && 0.3<ay<0.6 && 0<az<0.35 && p1>2)
{
  p2++; 
  // Serial.println("ball over");
 // p1=0; 
}

if (0<ax<0.6 && 0<ay<0.3 && 0<az<0.4 && p2==1)
{
  p2++; 
 // p1=0; 
}


if (p1>7)
{
    
     Serial.println("Ball over");
     String chucking="Ball over";

      LDateTime.getRtc(&rtc);
    if ((rtc - lrtc) >= per) {
      heartBeat();
      lrtc = rtc;
    }
    //Check for report datapoint status interval
    LDateTime.getRtc(&rtc1);
    if ((rtc1 - lrtc1) >= per1) {
      uploadstatus(chucking);
      //uploadstatus(1);
      lrtc1 = rtc1;
    }
  delay(1000);
    
     p1=0;
     
     
if (p2 == 2)
{
     Serial.println("chucking");
     String chucking="chucking";

delay(30000);
      LDateTime.getRtc(&rtc);
    if ((rtc - lrtc) >= per) {
      heartBeat();
      lrtc = rtc;
    }
    //Check for report datapoint status interval
    LDateTime.getRtc(&rtc1);
    if ((rtc1 - lrtc1) >= per1) {
      uploadstatus(chucking);
      //uploadstatus(1);
      lrtc1 = rtc1;
    }
    delay(20000);
     
    // p2=0;

}

}

Serial.println("p1");
Serial.println(p1);
Serial.println("p2");
Serial.println(p1);


if (0.00<ax<.200 && 0.4<ay<.99 && 0.0<az<0.5 && p3==0)
{
  p3++;
}
if (0.00<ax<.06 && 0.2<ay<0.8 && 0.02<az<0.09 && p3==1)
{
  p3++;
}
if (0.1<ax<0.6 && 0<ay<0.9 && 0.1<az<0.6 && p3==2)
{
  p3++;
}
if (0.10<ax<.5 && 0.5<ay<0.9 && 0.1<az<0.5 && p3==3)
{
  p3++;
}


if (240<ax<270 && .01<ay<.08 && 0.2<az<0.8 && p4==0)
{
p4++;
}
if (.0<ax<.6 && .00<ay<.08 && 0.2<az<0.8 && p4==1)
{
p4++;
}
if (0.7<ax<1.2 && .00<ay<0.3 && 0.0<az<0.3 && p4==2)
{
p4++;
}


if (0.0<ax<.4 && .04<ay<0.2 && 0.05<az<0.14 && p5==0)
{
p5++;
}
if (0.04<ax<.25 && .30<ay<0.60 && 0.05<az<0.35 && p5==1)
{
p5++;
}
if (0.00<ax<.07 && .5<ay<0.9 && 0.1<az<0.55 && p5==2)
{
p5++;
}
if (p3=4)
{
  Serial.println("Bowler");

  String chucking="Bowler";

      LDateTime.getRtc(&rtc);
    if ((rtc - lrtc) >= per) {
      heartBeat();
      lrtc = rtc;
    }
    //Check for report datapoint status interval
    LDateTime.getRtc(&rtc1);
    if ((rtc1 - lrtc1) >= per1) {
      uploadstatus(chucking);
      //uploadstatus(1);
      lrtc1 = rtc1;
    }
 //   delay(1000);
  
 
  p3=0;
}


if (p4==3)
{
  Serial.println("Mohanty");

  String chucking="Mohanty";

      LDateTime.getRtc(&rtc);
    if ((rtc - lrtc) >= per) {
      heartBeat();
      lrtc = rtc;
    }
    //Check for report datapoint status interval
    LDateTime.getRtc(&rtc1);
    if ((rtc1 - lrtc1) >= per1) {
      uploadstatus(chucking);
      //uploadstatus(1);
      lrtc1 = rtc1;
    }
    delay(1000);
   
  p4=0;
}

if (p5==3)
{
  Serial.println("Akthar");

 String chucking="Akthar";

      LDateTime.getRtc(&rtc);
    if ((rtc - lrtc) >= per) {
      heartBeat();
      lrtc = rtc;
    }
    //Check for report datapoint status interval
    LDateTime.getRtc(&rtc1);
    if ((rtc1 - lrtc1) >= per1) {
      uploadstatus(chucking);
      //uploadstatus(1);
      lrtc1 = rtc1;
    }
 //   delay(1000);


  
  delay(2000);
  p5=0;
}
}








//newly added code 
void getconnectInfo(){
  //calling RESTful API to get TCP socket connection
  c2.print("GET /mcs/v2/devices/");
  c2.print(DEVICEID);
  c2.println("/connections.csv HTTP/1.1");
  c2.print("Host: ");
  c2.println(SITE_URL);
  c2.print("deviceKey: ");
  c2.println(DEVICEKEY_TEMP);
  c2.println("Connection: close");
  c2.println();
  
  delay(500);

  int errorcount = 0;
  while (!c2.available())
  {
    Serial.println("waiting HTTP response: ");
    Serial.println(errorcount);
    errorcount += 1;
    if (errorcount > 10) {
      c2.stop();
      return;
    }
    delay(100);
  }
  int err = http.skipResponseHeaders();

  int bodyLen = http.contentLength();
  Serial.print("Content length is: ");
  Serial.println(bodyLen);
  Serial.println();
  char c;
  int ipcount = 0;
  int count = 0;
  int separater = 0;
  while (c2)
  {
    int v = c2.read();
    if (v != -1)
    {
      c = v;
      Serial.print(c);
      connection_info[ipcount]=c;
      if(c==',')
      separater=ipcount;
      ipcount++;    
    }
    else
    {
      Serial.println("no more content, disconnect");
      c2.stop();

    }
    
  }
  Serial.print("The connection info: ");
  Serial.println(connection_info);
  int i;
  for(i=0;i<separater;i++)
  {  ip[i]=connection_info[i];
  }
  int j=0;
  separater++;
  for(i=separater;i<21 && j<5;i++)
  {  port[j]=connection_info[i];
     j++;
  }
  Serial.println("The TCP Socket connection instructions:");
  Serial.print("IP: ");
  Serial.println(ip);
  Serial.print("Port: ");
  Serial.println(port);
  portnum = atoi (port);
  Serial.println(portnum);

} //getconnectInfo

void uploadstatus(String chucking){
  
  //calling RESTful API to upload datapoint to MCS to report LED status
  Serial.println("calling connection");
  LWiFiClient c2;  

  while (!c2.connect(SITE_URL, 80))
  {
    Serial.println("Re-Connecting to WebSite");
    delay(1000);
  }
  delay(100);

  upload_data = "chucking_1,,"+ chucking;
  
  int thislength = upload_data.length();
  HttpClient http(c2);
  c2.print("POST /mcs/v2/devices/");
  c2.print(DEVICEID);
  c2.println("/datapoints.csv HTTP/1.1");
  c2.print("Host: ");
  c2.println(SITE_URL);
  c2.print("deviceKey: ");
  c2.println(DEVICEKEY_TEMP);
  c2.print("Content-Length: ");
  c2.println(thislength);
  c2.println("Content-Type: text/csv");
  c2.println("Connection: close");
  c2.println();
  c2.println(upload_data);
  
  delay(500);

  int errorcount = 0;
  while (!c2.available())
  {
    Serial.print("waiting HTTP response: ");
    Serial.println(errorcount);
    errorcount += 1;
    if (errorcount > 10) {
      c2.stop();
      return;
    }
    delay(100);
  }
  int err = http.skipResponseHeaders();

  int bodyLen = http.contentLength();
  Serial.print("Content length is: ");
  Serial.println(bodyLen);
  Serial.println();
  while (c2)
  {
    int v = c2.read();
    if (v != -1)
    {
      Serial.print(char(v));
    }
    else
    {
      Serial.println("no more content, disconnect");
      c2.stop();

    }   
  }
}


void connectTCP(){
  //establish TCP connection with TCP Server with designate IP and Port
  c.stop();
  Serial.println("Connecting to TCP");
  Serial.println(ip);
  Serial.println(portnum);
  while (0 == c.connect(ip, portnum))
  {
    Serial.println("Re-Connecting to TCP");    
    delay(1000);
  }  
  Serial.println("send TCP connect");
  c.println(tcpdata);
  c.println();
  Serial.println("waiting TCP response:");
} //connectTCP

void heartBeat(){
  Serial.println("send TCP heartBeat");
  c.println(tcpdata);
  c.println();
    
} //heartBeat

boolean disconnectedMsg = false;


void printdatetime()
{
  LDateTime.getTime(&t); 
  Serial.print("Date ");
  Serial.print(t.day);
  Serial.print("/");
  Serial.print(t.mon);
  Serial.print("/");
  Serial.print(t.year);
  Serial.print(" Time ");
  Serial.print(t.hour);
  Serial.print(":");
  Serial.println(t.min);
}

