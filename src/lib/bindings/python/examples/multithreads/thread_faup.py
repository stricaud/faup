# -*- coding: utf-8 -*-
"""
Created on Tue Oct 22 13:41:14 2013

@author: slarinier
"""

import threading
class ThreadFaup(threading.Thread):
    def __init__(self,list_url,f):
        self.list_url=list_url
        threading.Thread.__init__(self)
        self.f=f
    def run(self):
        for url in self.list_url:
            self.f.decode(url)
            print self.f.get()
            
        