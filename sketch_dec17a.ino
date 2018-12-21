
#include <MicroGear.h>
#include <ESP8266WiFi.h>
#include <time.h>
#define APPID   "AiraCO"
#define KEY     "Il8fwPqTTXGGwcX"
#define SECRET  "1qCT5qfG77cYEa6GAh6YkpzRl"

#define ALIAS   "NodeMCU1"
#define TargetWeb "HTML_web"

#define D4 2   // TXD1

WiFiClient client;
MicroGear microgear(client);
const char* ssid     = "AndroidAP97FD";
const char* password = "megv3290";

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) 
{
    Serial.print("Incoming message --> ");
    msg[msglen] = '\0';
    Serial.println((char *)msg);
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) 
{
    Serial.println("Connected to NETPIE...");
    microgear.setAlias(ALIAS);
}

void setup() 
{
    microgear.on(MESSAGE,onMsghandler);
    microgear.on(CONNECTED,onConnected);

    Serial.begin(115200);
    Serial.println("Starting...");
   
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
       delay(250);
       Serial.print(".");
    }
    
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    microgear.init(KEY,SECRET,ALIAS);

    microgear.connect(APPID);

    configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    Serial.println("\nWaiting for time");
    while (!time(nullptr)) {
      Serial.print(".");
      delay(1000);
    }
    Serial.println("\nWaiting for sensor");
    while (!Serial.available()) {
      Serial.print(".");
      delay(1000);
    }
    Serial.println("");
}
char reciever[200];
int idx = 0;
void loop() 
{
   if (microgear.connected())
    {
       microgear.loop();
       //Serial.println("connected");

       if (Serial.available()) {
          char recieve = Serial.read();
          if(recieve == '|')
          {
//            Serial.println(reciever);
            char ans[idx+2];
            for(int i=0;i<idx;i++)ans[i] = reciever[i];
            ans[idx] = ' ';idx++;
            ans[idx] = '\0';
            Serial.println(ans);
            time_t now = time(nullptr);
            Serial.println(ctime(&now));
            idx = 0;
            String t = ctime(&now);
            String data = ans+t;
            char msg[128];
            data.toCharArray(msg,data.length()+1);
            microgear.chat(TargetWeb , msg);
          }
          else{
            reciever[idx++] = recieve;
          }

        }

        
//         char str[128];
//       sprintf(str, "%d", s);

//       //Serial.println(msg);    
//
//       microgear.chat(TargetWeb , msg);
       
  }
   else 
   {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
    delay(1500);
   }

}
