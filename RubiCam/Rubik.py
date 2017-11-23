# -*- coding: latin-1 -*-
# ################ graphical interface for the solver #####################################
# il faut installer:
#   numpy -> pip install numpy
#   opencv, télécharger et copier le fichier dans ...
#   enum -> pip install enum34

#Python 3.6
#from tkinter import *

#Python 2.7
from Tkinter import *

from video import *
from PIL import Image
from PIL import ImageTk
import face
import cubie
import solver as sv
import numpy as np
import cv2
import time


# ################################## some global variables and constants ###############################################

width = 78  # largeur des paves en pixels

facelet_id = [[[0 for col in range(3)] for row in range(3)] for face in range(6)]
facelet2_id = [[0 for col in range(3)] for row in range(3)]
colorpick_id = [0 for i in range(6)]
curcol = None
t = ("U", "R", "F", "D", "L", "B")
cols = ("yellow", "green", "red", "white", "blue", "orange")


########################################################################################################################
# ################################################ Diverse functions ###################################################
def colornum(f):
    """De la lettre de face vers numero """
    if f=='U': return 0     #UP    Yellow
    if f=='R': return 1     #Right Green
    if f=='F': return 2     #Front Red
    if f=='D': return 3     #Down  White
    if f=='L': return 4     #Left  Blue
    if f=='B': return 5     #Back Orange
    return 0

def traduire(f):
    """De la lettre couleur vers lettre de face """
    nf=[]
    for sf in f:
        if sf=="W": sf="D"
        if sf=="Y": sf="U"
        if sf=="B": sf="L"
        if sf=="R": sf="F"
        if sf=="G": sf="R"
        if sf=="O": sf="B"
        nf.append(sf)
    return nf

def show_text(txt):
    """la definition du cube """
    mylabel.config(text=txt)
    root.update_idletasks()

def show_text2(txt):
    """la solution """
    display2.insert(INSERT, txt)
    root.update_idletasks()
    
def create_facelet_rects(a):
    """Initializes the facelet grid on the canvas."""
    offset = ((1, 0), (2, 1), (1, 1), (1, 2), (0, 1), (3, 1))
    for f in range(6):
        for row in range(3):
            y = 20 + offset[f][1] * 3 * a + row * a
            for col in range(3):
                x = 10 + offset[f][0] * 3 * a + col * a
                facelet_id[f][row][col] = canvas.create_rectangle(x, y, x + a, y + a, fill="grey")
                if row == 1 and col == 1:
                    canvas.create_text(x + width // 2, y + width // 2, font=("", 14), text=t[f], state=DISABLED)
    for f in range(6):
        canvas.itemconfig(facelet_id[f][1][1], fill=cols[f])

def create_colorpick_rects(a):
    """Initializes the "paintbox" on the canvas"""
    global curcol
    global cols
    for i in range(6):
        x = i*(a//2+5) + 7*a
        y = 7*a
        colorpick_id[i] = canvas.create_rectangle(x, y, x + a//2, y + a//2, fill=cols[i])
        canvas.create_text(x + a // 4, y + a // 4, font=("", 14), text=t[i], state=DISABLED)
        canvas.itemconfig(colorpick_id[0], width=4)
        curcol = cols[0]
    
def draw_face(fcamstr):
    """Dessine la face vue par la camera"""
    offset = ((1, 0), (2, 1), (1, 1), (1, 2), (0, 1), (3, 1))
    a=40
    idx=0
    for row in range(3):
            y = 40  + row * a
            for col in range(3):
                x = 70  + col * a
                facelet2_id[row][col] = canvas.create_rectangle(x, y, x + a, y + a, fill="grey")
                canvas.itemconfig(facelet2_id[row][col], fill=cols[colornum(fcamstr[idx])])
                idx+=1

def get_definition_string():
    """Generates the cube definition string from the facelet colors."""
    color_to_facelet = {}
    for i in range(6):
        color_to_facelet.update({canvas.itemcget(facelet_id[i][1][1], "fill"): t[i]})
    s = ''
    for f in range(6):
        for row in range(3):
            for col in range(3):
                s += color_to_facelet[canvas.itemcget(facelet_id[f][row][col], "fill")]
    return s


##################################################################################################
# ################################# Functions pour disque  sauver lire     #######################


def fsave():
    global soluce
    with open('solution.txt',"w") as f:
         f.write(soluce+"\n")
    print ("solution.txt saved")

def fsave_cube():
    global cubestring
    with open('cube.txt',"w") as f:
         f.write(cubestring+"\n")
    print ("cube.txt saved")

def save_seting():
    global taille,margin,cx,cy
    with open('seting.txt',"w") as f:
         f.write(str(taille)+","+str(margin)+","+str(cx)+","+str(cy))
    print ("seting.txt saved")
    
def load_seting():
    global taille,margin,cx,cy,regions
    with open('seting.txt',"r") as f:
         s=f.read()
         sl=s.split(",")
         taille=int(sl[0])
         margin=int(sl[1])
         cx=int(sl[2])
         cy=int(sl[3])
         #print sl
         regions = makezones(cx,cy,taille,margin)
    print ("seting.txt read")    

def save_coldef():
    global color_definition_list
    with open('coldef.txt',"w") as f:
        for cd in color_definition_list:
             f.write(str(cd)+"\n")
    print ("coldef.txt saved")
    
def load():
    """a partir du fichier."""   
    global cubestring
    cstr=""
    display2.delete(1.0, END)  # clear output window
    nb_label.config(text="")
    with open('cube.txt') as f:
            print("read from cube.txt")
            cstr=f.read().strip('\n');
            print(cstr)
    show_text(cstr)
    idx = 0
    for f in range(6):
        for row in range(3):
            for col in range(3):
                canvas.itemconfig(facelet_id[f][row][col], fill=cols[colornum(cstr[idx])])
                idx+=1
    cubestring=cstr
    
      
#################################################################################################
# ################################# Functions to change the facelet colors ######################

def clean():
    global cubestring
    """Restores the cube to a clean cube."""
    display2.delete(1.0, END)  # clear output window
    cubestring="UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB"
    show_text(cubestring)
    nb_label.config(text="")
    for f in range(6):
        for row in range(3):
            for col in range(3):
                canvas.itemconfig(facelet_id[f][row][col], fill=canvas.itemcget(facelet_id[f][1][1], "fill"))


def random():
    """Generates a random cube and sets the corresponding facelet colors."""
    global cubestring
    display2.delete(1.0, END)  # clear output window
    cc = cubie.CubieCube()
    cc.randomize()
    fc = cc.to_facelet_cube()
    cubestring=str(fc)
    show_text(cubestring)
    nb_label.config(text="")
    idx = 0
    for f in range(6):
        for row in range(3):
            for col in range(3):
                canvas.itemconfig(facelet_id[f][row][col], fill=cols[fc.f[idx]] )
                idx += 1
                
#####################################################################################################
# ################################# BOUTONS             #############################################

def bzoomin():
    global taille,imglar,imghaut,margin,regions,cx,cy
    if taille<140:
                taille+=4
                regions = makezones(cx, cy, taille, margin)
def bzoomout():
    global taille,imglar,imghaut,margin,regions,cx,cy
    if taille>80:
                taille-=4
                regions = makezones(cx, cy, taille, margin)
def bnavleft():
    global taille,imglar,imghaut,margin,regions,cx,cy
    if cx>25:
                cx-=5
                regions = makezones(cx, cy, taille, margin)
def bnavright():
    global taille,imglar,imghaut,margin,regions,cx,cy
    if cx<imglar-25:
                cx+=5
                regions = makezones(cx, cy, taille, margin)
def bnavup():
    global taille,imglar,imghaut,margin,regions,cx,cy
    if cy>10:
                cy-=5
                regions = makezones(cx, cy, taille, margin)
def bnavdown():
    global taille,imglar,imghaut,margin,regions,cx,cy
    if cy<imghaut:
                cy+=5
                regions = makezones(cx, cy, taille, margin)
def bnext():
    global look
    if look<9: look+=1
    else: look=1

def bkeepface():
    global f
    #print(f)
    f_stdr=traduire(f)
    print(f_stdr)
    display2.delete(1.0, END)  # clear output window
    nb_label.config(text="")
    facestring=""
    for c in f_stdr: facestring+=str(c)
    show_text(facestring)
    draw_face(facestring)   
    insert_face(facestring)
    
def bdefinir():
    global curcol
    global color_definition_list
    (ok,f,frgb,fhsv,vis)=cam_capture(regions,look)
    if ok:
        color_definition_list.append((frgb,fhsv,curcol))
        print(frgb,fhsv,curcol)
        
    
def insert_face(cbs):
    global cubestring
    centre=cbs[4]
    if centre=='U':f=0
    if centre=='R':f=1
    if centre=='F':f=2
    if centre=='L':f=4   
    if centre=='D':f=3
    if centre=='B':f=5
    print("insert Face "+centre)
     
    idx=0
    for row in range(3):
            for col in range(3):
                canvas.itemconfig(facelet_id[f][row][col], fill=cols[colornum(cbs[idx])])
                idx+=1
    cubestring=get_definition_string()
    show_text(cubestring)

##########################################################################################
# ################################### Taches periodiques      ############################ 
def grab():
    global panel
    #campic= cam_grabimg()
    (ok,f,frgb,fhsv,campic)=cam_capture(regions,look)
    rgblabel.config(text="BGR "+str(frgb))
    hsvlabel.config(text="HSV "+str(fhsv))
    image = cv2.resize(campic, (480,360)) 
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    image = Image.fromarray(image)
    image = ImageTk.PhotoImage(image)
    panel.configure(image=image)
    panel.image = image

def looptask():
    global f
    global look
    global video_off
    global frgb,fhsv
    global panel

    if not video_off:     
            (ok,f,frgb,fhsv,campic)=cam_capture(regions,look)
            rgblabel.config(text="BGR "+str(frgb))
            hsvlabel.config(text="HSV "+str(fhsv))
            image = cv2.resize(campic, (480,360)) 
            image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
            image = Image.fromarray(image)
            image = ImageTk.PhotoImage(image)
            panel.configure(image=image)
            panel.image = image
            if ok:
                root.after(80,looptask)
       
def video():
    global video_off
    global taille,imglar,imghaut,margin,regions,cx,cy
    global caminitok
    
    if video_off:
        video_off=False
        if not caminitok:
            (imglar,imghaut)=cam_init(1)
            caminitok=True
            # definir 9 zones carees d'analyse
            taille=100
            (cx,cy)=(imglar//2,imghaut//2)
            margin=16
            regions = makezones(cx,cy,taille,margin)

        looptask()    
    else:
        cam_close()
        video_off=True
        
        
##########################################################################################
# ################################### Edit the facelet colors ############################

def click(event):
    """Defines how to react on left mouse clicks"""
    global curcol
    global cubestring
    idlist = canvas.find_withtag("current")
    
    if len(idlist) > 0:
        if idlist[0] in colorpick_id:
            curcol = canvas.itemcget("current", "fill")
            for i in range(6):
                canvas.itemconfig(colorpick_id[i], width=1)
            canvas.itemconfig("current", width=5)
        else:
            canvas.itemconfig("current", fill=curcol)
            cubestring=get_definition_string()
            show_text(cubestring)

##################################################################################################
# ############################### Solve the displayed cube  ######################################

def solve():
    global soluce
    global cubestring
    
    display2.delete(1.0, END)  # clear output window
    print("resoudre:"+cubestring)
    soluce = sv.solve(cubestring, 20, 2)
    if soluce.find("Error")<0:
        show_text2(soluce)
        nb_label.config(text=str(len(soluce)//3)+ " rotations")
    else:
        show_text2("Cube impossible")
    print("solution:"+soluce)
    return
            
###########################################################################################
# ###################################### Generate and display the TK_widgets ##############

  
global shotdir 
global save_ok
global taille,imglar,imghaut,margin,regions,cx,cy
global look
global video_off


save_ok=False
shotdir = "."
soluce=""
cubestring=""
look=1
video_off=True
caminitok=False
frgb=[]
fhsv=[]
color_definition_list=[]

root = Tk()
root.wm_title("Solver")
canvas = Canvas(root, width=18 * width + 60, height=9 * width + 40)

#image = Image.open('image.png')
image = Image.new('RGB',(480,360))
image = ImageTk.PhotoImage(image)

canvas.pack()


bsolve = Button(text="Solution", height=2, width=12, relief=RAISED, command=solve)
bsolve_window = canvas.create_window(10 + 8.5 * width , 45, anchor=NW, window=bsolve)

bline=15+320+50
bvideo = Button(text="Video\non/off", height=3, width=10, relief=RAISED, command=video)
bvideo_window = canvas.create_window(30 + 12 * width, bline, anchor=NW, window=bvideo)
bkeepface = Button(text="Capturer \ncette face", height=3, width=10, relief=RAISED, command=bkeepface)
bbkeepface_window = canvas.create_window(120 + 12 * width, bline, anchor=NW, window=bkeepface)
bnext = Button(text="Analyse\nsuivant", height=3, width=10, relief=RAISED, command=bnext)
bnext_window = canvas.create_window(210 + 12 * width, bline, anchor=NW, window=bnext)
bdefinir = Button(text="Definir\ncouleur", height=3, width=10, relief=RAISED, command=bdefinir)
bdefinir_window = canvas.create_window(300 + 12 * width, bline, anchor=NW, window=bdefinir)

rgblabel = Label(text="", bg=root.cget('bg'), font=("Arial", 10))
rgblabel_window = canvas.create_window(390+12 * width, bline, anchor=NW, window=rgblabel)
hsvlabel = Label(text="", bg=root.cget('bg'), font=("Arial", 10))
hsvlabel_window = canvas.create_window(390 +12 * width, 25+bline, anchor=NW, window=hsvlabel)


#--
bline=15+320+150
bzoomin = Button(text="zoom +", height=2, width=10, relief=RAISED, command=bzoomin)
bzoomin_window = canvas.create_window(30 + 12 * width, bline, anchor=NW, window=bzoomin)
bzoomout = Button(text="zoom -", height=2, width=10, relief=RAISED, command=bzoomout)
bzoomout_window = canvas.create_window(120 + 12 * width, bline, anchor=NW, window=bzoomout)

bnavleft = Button(text="<", height=2, width=6, relief=RAISED, command=bnavleft)
bnavleft_window = canvas.create_window(210 + 12 * width, bline, anchor=NW, window=bnavleft)
bnavright = Button(text=">", height=2, width=6, relief=RAISED, command=bnavright)
bnavright_window = canvas.create_window(340 + 12 * width, bline, anchor=NW, window=bnavright)

bnavup = Button(text="up", height=2, width=6, relief=RAISED, command=bnavup)
bnavup_window = canvas.create_window(275+ 12 * width, bline-25, anchor=NW, window=bnavup)
bnavdown = Button(text="v", height=2, width=6, relief=RAISED, command=bnavdown)
bnavdown_window = canvas.create_window(275+ 12 * width, bline+25, anchor=NW, window=bnavdown)
#--


display2 = Text(height=2, width=40,font=("Arial", 14), wrap="word")
text_window2 = canvas.create_window(6.4 * width,100, anchor=NW, window=display2)

mylabel = Label(text="", bg=root.cget('bg'), font=("Arial", 9))
mylabel_window = canvas.create_window(6.4 * width,15, anchor=NW, window=mylabel)
nb_label = Label(text="", bg=root.cget('bg'), font=("Arial", 9))
nb_label_window = canvas.create_window(6.4 * width,50, anchor=NW, window=nb_label)


#fenetre pour l'image camera
panel = Label(image=image,bg='black')
panel_window = canvas.create_window(30+12 * width, 15, anchor=NW, window=panel)
panel.image = image


menubar = Menu(root)

menu1 = Menu(menubar, tearoff=0)
menu1.add_command(label="Charger cube", command=load)
menu1.add_command(label="Reset", command=clean)
menu1.add_command(label="Random", command=random)
menu1.add_command(label="Sauver cube", command=fsave_cube)
menu1.add_command(label="Sauver solution", command=fsave)
menubar.add_cascade(label="Cube", menu=menu1)
menu2 = Menu(menubar, tearoff=0)
menu2.add_command(label="Sauver", command=save_seting)
menu2.add_command(label="Charger", command=load_seting)
menu2.add_command(label="Sauver color def", command=save_coldef)
menubar.add_cascade(label="Réglages", menu=menu2)

root.config(menu=menubar)

canvas.bind("<Button-1>", click)

create_facelet_rects(width)
create_colorpick_rects(width)

clean()



root.after(100,looptask)
root.mainloop()
########################################################################################################################

