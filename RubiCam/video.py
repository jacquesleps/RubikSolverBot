# -*- coding: latin-1 -*-
'''
Video capture sample.
Version Python 2.7 et3  (xrange -> range)
'''

import numpy as np
import cv2

fnt = cv2.FONT_HERSHEY_PLAIN

#--------------------------------------------------------------------
# active la camera

def create_capture(source = 0):
    cap = None
    cap = cv2.VideoCapture(source)
    if cap is None or not cap.isOpened():
        print( 'Warning: unable to open video source: ', source)

    return cap
#--------------------------------------------------------------------
# decide la couleur sur base de rgb et hsv

def MycolorByRGB(*args):

    (rgb, hsv) = args
    (h, s, v) = hsv
    (blue, green, red) = rgb
    
    green = float(max(green, 1))
    red = float(max(red, 1))
    blue = float(max(blue, 1))

    moy=(red+green+blue)//3
    sumerr=abs(red-moy)+abs(green-moy)+abs(blue-moy)
    if sumerr<30:
    #if green>120 and red>120 and blue>120 and (s<50 or v==0 or v>240):
        return 'W'

    if blue / red > 1.7 and blue / green > 1.2: 
        return 'B'
    elif green / red > 1.8:
        return 'G'

    if h > 150 or h < 2:
        return 'R'
    elif h < 20:
        return 'O'
    elif h < 50:
        return 'Y'

    if v==0:
        return 'W'

    return '?'
#--------------------------------------------------------------------
# couleur moyenne
def averageRGB(img):
    red = 0
    green = 0
    blue = 0
    num = 1
    for y in range(len(img)):
        if y%10 == 0:
            a = img[y]
            for x in range(len(a)):
                if x%10 == 0:
                    b = img[y][x]
                    num += 1
                    red += b[0]
                    green += b[1]
                    blue += b[2]
    red /= num
    green /= num
    blue /= num
    return (int(red), int(green), int(blue))
#--------------------------------------------------------------------
# maximum de l'histogramme
def histMode(hist, maxAmt):
    bin_count = int(hist.shape[0])
    maxAmount = int(hist[0])
    maxIndex = 0
    numZero = 0
    numTotal = 0
    for i in range(bin_count):
        h = int(hist[i])
        if h == 0: numZero += 1
        numTotal += 1
        if h > maxAmount:
            maxIndex = i
            maxAmount = h
    val = int(maxAmt * maxIndex / bin_count)
    return val

#--------------------------------------------------------------------
def makezones(cx, cy, taille, margin):
    regions = [ ]
    for row in range(3):
        ccy=cy+taille*(row-1)
        for col in range(3):
                ccx=cx+taille*(col-1)
                regions.append((ccx - taille//2 + margin,
                                 ccy - taille//2 + margin,
                                 ccx+ taille//2 - margin,
                                 ccy+taille//2 - margin))
    return regions
#--------------------------------------------------------------------    
def rgb_fromcode(codecoul):
    if codecoul=='W': return (255,255,255)
    if codecoul=='Y': return (0,255,255)
    if codecoul=='R': return (0,0,255)
    if codecoul=='B': return (255,0,0)
    if codecoul=='G': return (0,255,0)
    if codecoul=='O': return (0,128,255)
    return (0,0,0)

#--------------------------------------------------------------------    
def cam_init(camid):
    
    global cap
    print ("OpenCV "+cv2.__version__+ " ready")
    cap = create_capture(camid)
    ret, vis = cap.read()
    imghaut=vis.shape[0]
    imglar=vis.shape[1]
    # pas de fenetre video
    #cv2.namedWindow('video',cv2.WINDOW_NORMAL)
    #cv2.moveWindow('video', 0, 0)

    print ("Image size "+str(imglar)+"x"+str(imghaut)) 
    print("ESC to exit video window")   
    
    
    return (imglar,imghaut)

#--------------------------------------------------------------------    
def cam_capture(regions,look):
        global cap
        
        ret, vis = cap.read()
        #cv2.imshow('video', vis)
        imghaut=vis.shape[0]
        imglar=vis.shape[1]
        
        hsv = cv2.cvtColor(vis, cv2.COLOR_BGR2HSV)
        #note: en HSV, H va de 0 a 180
        # ce masque elimine le blanc
        mask = cv2.inRange(hsv, np.array((0., 50., 60.)),
                            np.array((180., 255., 255.)))
        # ce masque prends tout
        mask2 = cv2.inRange(hsv, np.array((0., 0., 0.)), 
                             np.array((180., 255., 255.)))
        #cv2.imshow('mask', mask)
        #cv2.imshow('mask2', mask2)

        # pour chaque zone, analyser
        numz=0
        texts = []
        face=[]

        for (x0, y0, x1, y1) in regions:
            numz+=1
            (w, h) = (x1 - x0, y1 - y0)
            (x0m, y0m, x1m, y1m) = (int(x0 + w/5), int(y0 + h/5), int(x1 - w/5), int(y1 - h/5))

            hsv_roi = hsv[y0m:y1m, x0m:x1m]
            mask_roi = mask[y0m:y1m, x0m:x1m]
            mask_roi2 = mask2[y0m:y1m, x0m:x1m]
            #calculer 3 histogrammes
            histHue = cv2.calcHist( [hsv_roi], [0], mask_roi, [50], [0, 180] )
            histSat = cv2.calcHist( [hsv_roi], [1], mask_roi2, [50], [0, 180] )
            histVal = cv2.calcHist( [hsv_roi], [2], mask_roi2, [50], [0, 180] )
            cv2.normalize(histHue, histHue, 0, 255, cv2.NORM_MINMAX);
            histHue = histHue.reshape(-1)
            histSat = histSat.reshape(-1)
            histVal = histVal.reshape(-1)
            #prendre les valeurs les plus frequentes
            hue = histMode(histHue, 180.)
            sat = histMode(histSat, 255.)
            val = histMode(histVal, 255.)
            avghsv = (hue, sat, val)

            #RGB, prendre la moyenne
            rgb_inRegion = vis[y0m:y1m, x0m:x1m]
            avgrgb = averageRGB(rgb_inRegion)

            if numz==look:
                info="HSV"+str(avghsv)+" BGR"+str(avgrgb)
                full_rgb=avgrgb
                full_hsv=avghsv

            #decider la couleur

            color = MycolorByRGB(avgrgb, avghsv)

            #ajoute les infos couleur a la liste
            texts.append(((x0+x1) / 2, (y0 + y1) / 2, str(color)))
            face.append(color)
            
            # dessine un carre blanc pour montrer la zone dans l'image
            colcadre=(255, 255, 255)
            if color=='W' or color=='Y':colcadre=(0, 0, 0)
            ep=1
            if numz==look:ep=2
            cv2.rectangle(vis, (x0, y0), (x1, y1), colcadre,ep)
            
        #retourne l'image gauche droite
        #vis = vis[::,::-1].copy()
        
        # affiche les couleurs dans un carre noir, construit la liste des faces
        fill = (255,255,255)   
        for (x, y, color) in texts:    
            ix=int(x)
            iy=int(y)
            fillin = rgb_fromcode(color)
            #cv2.rectangle(vis, (ix-10, iy-10), (ix+10, iy+10), fillin, -1)
            cv2.circle(vis,(ix, iy), 22, fillin, -1)
            #cv2.putText(vis, color, (ix-5, iy+4), fnt, 1, fill)
             

        #affiche le tout
        #cv2.imshow('video', vis)

        # teste le clavier
        ch = 0xFF & cv2.waitKey(1)      
        if ch == 27:
            cam_close()
            return (False,face,full_rgb,full_hsv,vis)
        
        return (True,face,full_rgb,full_hsv,vis)
               
def cam_close():
            
    cv2.destroyAllWindows()
