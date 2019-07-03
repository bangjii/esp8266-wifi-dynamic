# esp8266-wifi-dynamic
Dynamic configuration SSID and password for esp8266 arduino.
you can edit ssid and pwd to default configuration

```
  String ssid = "Your SSID";
  String pwd = "ssidPassword";
```

if you want to connect different SSID and password you can add it from serial. Just connect your esp8266 through serial cable and send command :

```
command:
  save(SSID name,SSIDpassword)
example: 
  save(Redmi,12345678)
```
without quote or double quote, and dont use space between SSID(comma)pasword.
To erase existing SSID and password you can using this command :

```
  hapusWifi()
```
