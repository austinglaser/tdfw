import Tkinter
from PIL import ImageTk, Image

#This creates the main window of an application
top = Tkinter.Tk()
top.title("Join")
top.geometry("300x300")
top.configure(background='grey')

path = "test.png"

#Creates a Tkinter-compatible photo image, which can be used everywhere Tkinter expects an image object.
img = ImageTk.PhotoImage(Image.open(path))

#The Label widget is a standard Tkinter widget used to display a text or image on the screen.
panel = Tkinter.Label(top, image = img)

#The Pack geometry manager packs widgets in rows or columns.
panel.pack(side = "bottom", fill = "both", expand = "yes")

#Start the GUIe
top.mainloop()