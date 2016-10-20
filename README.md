# RPI-ESP8266-TORNADO-SERVER

#### This project aim to setup Tornado Web Server in Rpi and create a Web Socket with NodeMCU.

#### Tornado Webserver Setup on Raspberry
I follow [This Guide](https://developer.mbed.org/cookbook/Websockets-Server) to setup and testing Tornado Webserver.
```sh
sudo apt-get update
sudo pip3 install tornado
```
After setup is complete, Run [test-Server.py](https://github.com/Project-MAR/RPI-ESP8266-TORNADO-SERVER/blob/master/test-Server.py) to start a Tornado Server. Use [HelloWorld.html](https://github.com/Project-MAR/RPI-ESP8266-TORNADO-SERVER/blob/master/HelloWorld.html) (open it with your browser) to confirm that your server is run correctly. Becarefull, Your Web Browser must support Web Socket, for me, I use Google Chrome.


#### Web Socket Client Setup on NodeMCU

Start with [This project](https://github.com/Links2004/arduinoWebSockets), I can create NodeMCU Web Socket Client. Download and run [test-Client.ino](https://github.com/Project-MAR/RPI-ESP8266-TORNADO-SERVER/blob/master/test-Client.ino) then enjoy with basic Web Socket Communication.
