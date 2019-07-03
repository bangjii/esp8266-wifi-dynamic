/*
 Save and load from serial to eeprom
 modified 3 juli 2019
 by Hudzaifah.
*/
#include <EEPROM.h>

void clearMem(){
  for (int i = 0 ; i < EEPROM.length() ; i++){
    EEPROM.write(i, 0);
    Serial.print(".");
  }
  EEPROM.commit();
  Serial.println();
  Serial.println("Clear done!");
}

void saveString(char add,String data){
  int _size = data.length();
  int i;
  for(i=0;i<_size;i++){
    EEPROM.write(add+i,data[i]);
  }
  EEPROM.write(add+_size,'\0');
  EEPROM.commit();
}

String loadString(char add){
  int i;
  char data[100];
  int len=0;
  unsigned char k;
  k=EEPROM.read(add);
  while(k != '\0' && len<500){    
    k=EEPROM.read(add+len);
    data[len]=k;
    len++;
  }
  data[len]='\0';
  return String(data);
}

String bufClient;
void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  //clearMem();
  String rxData;
  rxData = loadString(10);
  if (rxData != ""){
    int len = rxData.length();
    int koma = rxData.indexOf(",");
    String ssid = rxData.substring(0, koma);
    String pwd = rxData.substring(koma + 1, len);      
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(pwd);
    
  } else {
    Serial.println("SSID: CIBUBUR");
    Serial.println("Password: gampangcuy 123456");
  }
}

void loop() {
  if (Serial.available()) {
    bufClient = "";
    while (Serial.available()) {
      bufClient += Serial.readString();       
    }
  }
  if (bufClient != ""){
    int startVal = bufClient.indexOf("(");
    int endVal = bufClient.indexOf(")");
    String cmd = bufClient.substring(0,startVal);
    String val = bufClient.substring(startVal + 1,endVal);
    if (cmd == "save"){
      Serial.print("Saving WiFi: @[address 10]");
      Serial.println(val);
      saveString(10, val);  //Address 10 and String type data
    }
    else if(cmd == "hapusWifi"){
      clearMem();
    }
    bufClient = "";
    cmd = "";
    val = "";
  }
}
