# ################ A simple graphical interface which communicates with the server #####################################

from tkinter import *
import socket
import face
import cubie
import solver as sv


# ################################## some global variables and constants ###############################################

width = 94  # width of a facelet in pixels
facelet_id = [[[0 for col in range(3)] for row in range(3)] for face in range(6)]
colorpick_id = [0 for i in range(6)]
curcol = None
t = ("U", "R", "F", "D", "L", "B")
cols = ("yellow", "green", "red", "white", "blue", "orange")


########################################################################################################################
# ################################################ Diverse functions ###################################################

def show_text(txt):
    """Displays messages."""
    #print(txt)
    display.delete(1.0,END)
    display.insert(INSERT, txt)
    root.update_idletasks()

def show_text2(txt):
    """Displays messages."""
    #print(txt) 
    display2.insert(INSERT, txt)
    root.update_idletasks()
    
def create_facelet_rects(a):
    """Initializes the facelet grid on the canvas."""
    offset = ((1, 0), (2, 1), (1, 1), (1, 2), (0, 1), (3, 1))
    for f in range(6):
        for row in range(3):
            y = 10 + offset[f][1] * 3 * a + row * a
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
        x = (i % 3)*(a+5) + 7*a
        y = (i // 3)*(a+5) + 7*a
        colorpick_id[i] = canvas.create_rectangle(x, y, x + a, y + a, fill=cols[i])
        canvas.create_text(x + width // 2, y + width // 2, font=("", 14), text=t[i], state=DISABLED)
        canvas.itemconfig(colorpick_id[0], width=4)
        curcol = cols[0]


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
########################################################################################################################

# ############################### Solve the displayed cube  ############################################################

def solve():
    global soluce
    global cubestring
    
    display2.delete(1.0, END)  # clear output window
    print("cube a resoudre:",cubestring)
    soluce = sv.solve(cubestring, 20, 3)
    show_text2(soluce)
    return


########################################################################################################################

# ################################# Functions ajoutées                     #############################################
def colornum(f):
    if f=='D': return 3     #Down  White
    if f=='U': return 0     #UP    Yellow
    if f=='R': return 1     #Right Green
    if f=='L': return 4     #Left  Blue
    if f=='F': return 2     #Front Red
    if f=='B': return 5     #Back Orange
    return 0

def fsave():
    global soluce
    with open('solution.txt',"w") as f:
         f.write(soluce+"\n")
    print ("solution.txt saved")
    
    
def load():
    """a partir du fichier."""
    """ W->D, Y->U, B->L, R->F, G->, O->B   """
    """ doit etre dans l'ordre Y G R W B O   """
    
    global cubestring
    cstr=""
    display2.delete(1.0, END)  # clear output window
    with open('cube.txt') as f:
            print("read from cube.txt")
            for line in f: cstr+=line.replace(",","").strip('\n')
                
    print(cstr)
    # convertir
    cstr=cstr.replace("W","D")
    cstr=cstr.replace("Y","U")
    cstr=cstr.replace("B","L")
    cstr=cstr.replace("R","F")
    cstr=cstr.replace("G","R")
    cstr=cstr.replace("O","B")
    print(cstr)
    show_text(cstr)
    idx = 0
    for f in range(6):
        for row in range(3):
            for col in range(3):
                canvas.itemconfig(facelet_id[f][row][col], fill=cols[colornum(cstr[idx])])
                idx+=1
    cubestring=cstr
    
########################################################################################################################

# ################################# Functions to change the facelet colors #############################################

def clean():
    """Restores the cube to a clean cube."""
    display2.delete(1.0, END)  # clear output window
    for f in range(6):
        for row in range(3):
            for col in range(3):
                canvas.itemconfig(facelet_id[f][row][col], fill=canvas.itemcget(facelet_id[f][1][1], "fill"))


def empty():
    """Removes the facelet colors except the center facelets colors."""
    display2.delete(1.0, END)  # clear output window
    for f in range(6):
        for row in range(3):
            for col in range(3):
                if row != 1 or col != 1:
                    canvas.itemconfig(facelet_id[f][row][col], fill="grey")


def random():
    """Generates a random cube and sets the corresponding facelet colors."""
    global cubestring
    display2.delete(1.0, END)  # clear output window
    cc = cubie.CubieCube()
    cc.randomize()
    fc = cc.to_facelet_cube()
    show_text(fc)
    cubestring=str(fc)
    idx = 0
    for f in range(6):
        for row in range(3):
            for col in range(3):
                canvas.itemconfig(facelet_id[f][row][col], fill=cols[fc.f[idx]] )
                idx += 1
########################################################################################################################

# ################################### Edit the facelet colors ##########################################################


def click(event):
    """Defines how to react on left mouse clicks"""
    global curcol
    idlist = canvas.find_withtag("current")
    if len(idlist) > 0:
        if idlist[0] in colorpick_id:
            curcol = canvas.itemcget("current", "fill")
            for i in range(6):
                canvas.itemconfig(colorpick_id[i], width=1)
            canvas.itemconfig("current", width=5)
        else:
            canvas.itemconfig("current", fill=curcol)
########################################################################################################################

#  ###################################### Generate and display the TK_widgets ##########################################
root = Tk()
root.wm_title("Solver")
canvas = Canvas(root, width=12 * width + 20, height=9 * width + 20)
canvas.pack()

soluce=""
cubestring=""

bload = Button(text="Load", height=3, width=10, relief=RAISED, command=load)
bload_window = canvas.create_window(-10 + 6.5 * width,  10 + 1.5 * width, anchor=NW, window=bload)

bsave = Button(text="Save", height=3, width=10, relief=RAISED, command=fsave)
bsave_window = canvas.create_window(170 + 6.5 * width , 10 + 1.5 * width, anchor=NW, window=bsave)

bsolve = Button(text="Solve", height=3, width=10, relief=RAISED, command=solve)
bsolve_window = canvas.create_window(80 + 6.5 * width , 10 + 1.5 * width, anchor=NW, window=bsolve)

bclean = Button(text="Reset", height=3, width=10, relief=RAISED, command=clean)
bclean_window = canvas.create_window(-10 + 6.5 * width, 80 + 1.5 * width, anchor=NW, window=bclean)

bempty = Button(text="Empty", height=3, width=10, relief=RAISED, command=empty)
bempty_window = canvas.create_window(170 + 6.5 * width, 80 + 1.5 * width, anchor=NW, window=bempty)

brandom = Button(text="Random", height=3, width=10, relief=RAISED, command=random)
brandom_window = canvas.create_window(80 + 6.5 * width, 80 + 1.5 * width, anchor=NW, window=brandom)

display = Text(height=1, width=54)
text_window = canvas.create_window(-10 + 6.5 * width, -5 + .5 * width, anchor=NW, window=display)

display2 = Text(height=3, width=66)
text_window2 = canvas.create_window(-10 + 6.5 * width, 30 + .5 * width, anchor=NW, window=display2)

canvas.bind("<Button-1>", click)
create_facelet_rects(width)
create_colorpick_rects(width)

root.mainloop()
########################################################################################################################

