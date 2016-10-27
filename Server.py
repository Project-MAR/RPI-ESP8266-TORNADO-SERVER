#!/usr/bin/python3

import tornado.httpserver
import tornado.websocket
import tornado.ioloop
import tornado.web
import socket

g_clients  = []

class IndexHandler(tornado.web.RequestHandler):
    @tornado.web.asynchronous
    def get(self):
        print ('webpage connect')
        self.render("index.html")

class WebWSHandler(tornado.websocket.WebSocketHandler):

    def check_origin(self, origin):
        return True

    def open(self):
        print ('socket from web connect')
        
        if self not in g_clients:
            print('append connection')
            g_clients.append(self)
        print('')
        print ('Websocket Connect')
        print('Client number: ' + str(len(g_clients)))
      
    def on_message(self, message):
        print ('message received: %s' % message)

        if len(g_clients) > 0:
            print('Foward to Port 8880')
            self.send_message_to_all(message)
            #self.forward_message(message)
            
            print('>>> Client List <<<')
            for c in g_clients:
                print(str(c))
        
    def on_close(self):
        if self in g_clients:
            print ('websocket closed: ' + str(self))
            g_clients.remove(self)

        print('>>> Client List <<<')
        for c in g_clients:
            print(str(c))

    def send_message_to_all(self, message):
        for c in g_clients:
            c.write_message(message)

WebApp = tornado.web.Application([
    (r'/', IndexHandler),
    (r'/web_ws', WebWSHandler),
])
 
if __name__ == "__main__":

    #tornado.options.parse_command_line()
    
    #sockets = tornado.netutil.bind_sockets(8880)
    #tornado.process.fork_processes(0)
    http_server = tornado.httpserver.HTTPServer(WebApp)
    #http_server.add_sockets(sockets)
    #http_server.listen(80)

    http_server.bind(80)
    http_server.bind(8880)
    http_server.start(num_processes = 1)

    myIP = socket.gethostbyname(socket.gethostname())
    print ('*** Websocket Server Started at %s***' % myIP)

    tornado.ioloop.IOLoop.instance().start()

