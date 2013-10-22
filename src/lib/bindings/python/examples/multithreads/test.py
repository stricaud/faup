# -*- coding: utf-8 -*-
"""
Created on Tue Oct 22 13:41:00 2013

@author: slarinier
"""
import sys
from thread_faup import ThreadFaup
from pyfaup.faup import Faup
nbr_thread=sys.argv[1]
list_url=[]
with open(sys.argv[2],'r') as fd:
    for line in fd:
        line=line.replace('\r\n','')
        list_url.append(line)
f=Faup()
for i in range(0,int(nbr_thread)):
    t=ThreadFaup(list_url,f)
    t.start()
