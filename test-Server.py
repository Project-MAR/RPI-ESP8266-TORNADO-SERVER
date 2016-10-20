#!/usr/bin/python3

import tornado.httpserver
import tornado.websocket
import tornado.ioloop
import tornado.web
import socket
'''
This is a simple Websocket Echo server that uses the Tornado websocket handler.
Please run `pip install tornado` with python of version 2.7.9 or greater to install tornado.
This program will echo back the reverse of whatever it recieves.
Messages are output to the terminal for debuggin purposes. 
''' 
 
class WSHandler(tornado.websocket.WebSocketHandler):
    
    def check_origin(self, origin):
        #allow_origin = self.server.settings.get("websocket_allow_origin", "*")
        #if allow_origin == "*":
        #    return True
        return True

    def open(self):
        print ('new connection')
        self.write_message('Hello from Server')
      
    def on_message(self, message):
        print ('message received: %s' % message)
        # Reverse Message and send it back
        print ('sending back message: %s' %  message)
        self.write_message('echo-> ' + message)
 
    def on_close(self):
        print ('connection closed')
 
    def check_origin(self, origin):
        return True
 
application = tornado.web.Application([
    (r'/ws', WSHandler),
])
 
if __name__ == "__main__":
    http_server = tornado.httpserver.HTTPServer(application)
    http_server.listen(8880)
    myIP = socket.gethostbyname(socket.gethostname())
    print ('*** Websocket Server Started at %s***' % myIP)
    tornado.ioloop.IOLoop.instance().start()
