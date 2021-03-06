# RPI-ESP8266-TORNADO-SERVER
   
#### This project aim to setup Tornado Web Server in Rpi and create a Web Socket Client with NodeMCU.
---

#### System Overview
<img src="https://github.com/Project-MAR/RPI-ESP8266-TORNADO-SERVER/blob/master/img/systemOverview.png" width="600">

This Project try to build a Personal Local Server for IoT Project. Fornow, most of IoT solution use public server(free or paid). Trust if network is down, you lost all control for your IoT devices. The local IoT server Act as a center of the communication between user (web brownser) and the IoT Device(Node with control the light bulb). User can control the bulb from a traditional switch or a web brownser. Because there are two source of signals to control the IoT Device, so a NodeMCU will handle a state change (on/off) of the light bulb according to the incomming event. A Demo is avalable in [YouTube.](https://www.youtube.com/watch?v=OhDF2fYqwgc)


#### Tornado Webserver Setup on Raspberry

I follow [This Guide](https://developer.mbed.org/cookbook/Websockets-Server) to setup and testing Tornado Webserver.
```sh
sudo apt-get update
sudo pip3 install tornado
```
After setup is complete, Run [test-Server.py](https://github.com/Project-MAR/RPI-ESP8266-TORNADO-SERVER/blob/master/test-Server.py) to start a Tornado Server. Use [HelloWorld.html](https://github.com/Project-MAR/RPI-ESP8266-TORNADO-SERVER/blob/master/HelloWorld.html) (open it with your browser) to confirm that your server is run correctly. Becarefull, Your Web Browser must support Web Socket, for me, I use Google Chrome.

---
#### Web Socket Client Setup on NodeMCU

Start with [Links2004 Project](https://github.com/Links2004/arduinoWebSockets), Download and run [test-Client.ino](https://github.com/Project-MAR/RPI-ESP8266-TORNADO-SERVER/blob/master/test-Client.ino) then enjoy with basic Web Socket Communication.

---

#### Rename Rpi hostname and setup Bonjour

First.
```sh
sudo nano /etc/hostname
```
This file contains one line, Hostname. Edit which you want then save it.

Second.
```sh
sudo nano /etc/hosts
```
The second file also cantians the hostname but it's only use for some software. Find the line starting with 127.0.0.1 and change it

Third.
Setup Bonjour. This will allow you to access to RPi from hostname.local (adding .local after your hostname). RPi may have it installed already, but if not then.
```sh
sudo apt-get update
sudo apt-get install libnss-mdns
```

---

#### CAUTION: provisional headers are shown” in Chrome debugger
On page load websocket works, but after that when applied with click event, then its not working     
 - Type "chrome://net-internals" in the address bar and hit enter.
 - Open the page that is showing problems.
 - Go back to net-internals, click on events (###) and use the textfield to find the event related to your resource (use parts of the URL).
Finally, click on the event and see if the info shown tells you something.   

---

#### Make a Server run after startup

put [Tornado.Server](https://github.com/Project-MAR/RPI-ESP8266-TORNADO-SERVER/blob/master/Tornado.Server) in /etc/init.d 
then run
```sh
sudo update-rc.d Tornado.Server defaults
```
Test Tornado.Server service with this command
```sh
sudo /etc/init.d/Tornado.Server start
```
Or
```sh
sudo /etc/init.d/Tornado.Server stop

```
If you want to remove this service, just run.

```sh
sudo update-rc.d -f Tornado.Server remove
```
---

#### Resolve tornado-server.local with mDNS for ESP8266
with a help from mDNS library form [mrdunk](https://github.com/mrdunk/esp8266_mdns)   
I modified and make nodeMCU to find tornado-server.local   
Checkout at b58a8e3 for a test code that resolve tornado-server.local and get IP Address with mDNS   
```sh
git checkout b58a8e3
```
---

#### SSL Option
Create certificate and privateKey   
(For me, these files store in /var/tornado-server/keys/)   
```sh
sudo openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout privateKey.key -out certificate.crt
```

Modified Tornado Server Object

```python
http_server = tornado.httpserver.HTTPServer(WebApp, ssl_options={
        "certfile": "/var/tornado-server/keys/certificate.crt",
        "keyfile" : "/var/tornado-server/keys/privateKey.key",
    })
```

Copy certificate.crt to Computer and add this certificate into "Trusted Root Certificate"   
Then Upgrade webpage and NodeMCU to wss.   
   
##### Problem with mobile device
If https, Can't access https because browser don't truse the certificate.   
Read this   
   - [.local Server Certificate](https://www.w3.org/wiki/images/3/37/2016.w3c.breakout_session.dot-local-server-cert.p.pdf)
   - [SSL/TLS Certificates for Internal Servers](https://www.globalsign.com/en/blog/certificates-for-internal-servers/)
   - [HTTPS Certificate for internal use](http://stackoverflow.com/questions/616055/https-certificate-for-internal-use)
   - [SECURING YOUR ESP8266 SETUP](http://www.esp8266.com/viewtopic.php?f=6&t=2439)

If http with web WebSocket, Although WebSocket protocol has been standardized by the IETF as RFC 6455, it doesn't support in Android Browser.

---

#### AC Sensor for detecting load status   
SSL + AC_Sense + fix switch state.   
```sh
git checkout 10af5dd
```
Note: AC Sense (ON->OFF) response too slow because of RC constant. No problem but cause an effect like a bouncing from mechanical switch

---

