# -*- coding: cp1252 -*-
# Construction d'un fichier hex à partir d'une images jpg
# J.Lepot 2015

import os
from PIL import Image
from PIL import ImageDraw

logoname='img3.png'

largeur=128
hauteur=64

logo = Image.open(logoname)
print ("image chargée")

f=open("img.txt","w")
for l in range(0, hauteur):
    line=[]
    for x in range(0, largeur):
        r, g, b = logo.getpixel((x, l))
        #pix=(r+g+b)/3
        if r>127:line.append(0)
        else:line.append(1)

    s=""
    bits=0
    i=0
    cnt=0
    pw2=1
    bhex=[]
    for x in range(0, largeur):
        
        if line[x]==0:
            s+="."
            cnt+=pw2
        else:
            s+="#"
            
        bits+=1
        pw2=pw2*2

        if bits==8:
           cnt= int('{:08b}'.format(cnt)[::-1], 2)
           bhex.append(cnt)
           i+=1
           bits=0
           pw2=1
           cnt=0
    shex=""
    for  bb in bhex:
        shex+=format(bb,'02x')
    shex=shex.upper()
    #print (shex)
    f.write(shex+"\n\r")
    print (s)
    
f.close()





