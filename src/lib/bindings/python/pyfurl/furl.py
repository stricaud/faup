from functions import *

class Furl():    
    def __init__(self):
    	self.fh = furl_init()

    def decode(self, url):
        furl_decode(self.fh, url)
        
