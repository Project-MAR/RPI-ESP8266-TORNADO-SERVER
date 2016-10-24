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

forwardMsg = []

class IndexHandler(tornado.web.RequestHandler):
    @tornado.web.asynchronous
    def get(self):
        print ('webpage connect')
        self.render("index.html")
        #we don't need self.finish() because self.render() is fallowed by self.finish() inside tornado
        #self.finish()

class WebWSHandler(tornado.websocket.WebSocketHandler):

    def check_origin(self, origin):
        return True

    def open(self):
        print ('socket from web connect')
        #self.write_message('Hello from Server')
      
    def on_message(self, message):
        print ('message received: %s' % message)
        #self.write_message('WEB: OK')
        forwardMsg.append(message)
        #WS_forward = WSHandler(tornado.websocket.WebSocketHandler)
        #WS_forward.sendMsg(message)
        
    def on_close(self):
        print ('socket from web closed')
        print ('')

class WSHandler(tornado.websocket.WebSocketHandler):
    
    def check_origin(self, origin):
        return True

    def open(self):
        print ('websocket connect')
        #self.write_message('Hello from Server')
      
    def on_message(self, message):
        print ('message received: %s' % message)
        if(len(forwardMsg) == 0):
            #self.write_message('None')
            pass
        else:
            self.write_message(forwardMsg[len(forwardMsg) - 1])
            forwardMsg.remove(forwardMsg[len(forwardMsg) - 1])
 
    def on_close(self):
        print ('websocket closed')
        print ('')

    def sendMsg(self, message):
        self.write_message(message)

WebApp = tornado.web.Application([
    (r'/', IndexHandler),
    (r'/web_ws', WebWSHandler),
])

SocketApp = tornado.web.Application([
    (r'/ws', WSHandler),
])
 
if __name__ == "__main__":

    #tornado.options.parse_command_line()
    
    #sockets = tornado.netutil.bind_sockets(8880)
    #tornado.process.fork_processes(0)
    http_server = tornado.httpserver.HTTPServer(WebApp)
    #http_server.add_sockets(sockets)
    http_server.listen(80)

    #http_server.bind(80)
    #http_server.bind(8880)
    #http_server.start(0)

    socket_server = tornado.httpserver.HTTPServer(SocketApp)
    socket_server.listen(8880)

    myIP = socket.gethostbyname(socket.gethostname())
    print ('*** Websocket Server Started at %s***' % myIP)
    
    #tornado.ioloop.IOLoop.current().start()
    tornado.ioloop.IOLoop.instance().start()

    
