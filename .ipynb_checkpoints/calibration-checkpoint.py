## GUI for calibration
    #! Code doesn't work yet, use curve_fit.py

from scipy.optimize import curve_fit
import matplotlib.pyplot as plt
import numpy as np
import tkinter as tk  # replace with tkinter for python 3
from matplotlib import colors
from numba import jit
from tkinter import ttk
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import (
    FigureCanvasTkAgg, NavigationToolbar2Tk)
import matplotlib as mpl
mpl.use("TkAgg")

# real_weight=np.loadtxt("data.csv", delimiter=";")
# fmu_weight=range(len(real_weight))
mpl.rcParams['toolbar'] = 'None'  # erase buttons

def f(x, a, b):
	return a*x+b


def curvefit(ax, real_weight=np.array([0.0, 3.3, 21.0]), fmu_weight=np.array([6.8, 11.0, 24.6])):

    real_weight = real_weight*1E-3*9.81  # grams to Newtons
    fmu_weight = fmu_weight*1E-3*9.81  # grams to Newtons

    popt, pcov = curve_fit(f, fmu_weight, real_weight)

    ax.plot(fmu_weight, real_weight, 'o', label="data")
    ax.plot(fmu_weight, f(fmu_weight, *popt), 'r-',
             label='curve_fit: a=%5.3f, b=%5.3f' % tuple(popt))
    # plt.errorbar(fmu_weight, real_weight, yerr=1, xerr= 0, fmt="none")

LARGE_FONT = ("Verdana", 12)
NORM_FONT = ("Verdana", 10)


class base(tk.Tk):

    def __init__(self, *args, **kwargs):

        tk.Tk.__init__(self, *args, **kwargs)
        tk.Tk.wm_title(self, "Mandelbrot Renderer")

        container = tk.Frame(self)
        container.pack(side="top", fill="both", expand=True)
        container.grid_rowconfigure(0, weight=1)
        container.grid_columnconfigure(0, weight=1)

        self.frames = {}

        for F in (StartPage, MainPage):

            frame = F(container, self)

            self.frames[F] = frame

            frame.grid(row=0, column=0, sticky="nsew")

        self.show_frame(StartPage)

    def show_frame(self, cont):

        frame = self.frames[cont]
        frame.tkraise()


class StartPage(tk.Frame):

    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="Start Page", font=LARGE_FONT)
        label.pack(pady=10, padx=10)

        label2 = tk.Label(self, text="How many weights?")
        label2.pack(side=tk.LEFT)

        input1 = tk.Entry(self)
        input1.pack(side=tk.LEFT)

        # button1 = tk.Button(self, text="Process",command=setNofWeights())
        # button1.pack()
        
        button = tk.Button(self, text="Lets Begin",
                           command=lambda: controller.show_frame(MainPage))
        button.pack()

class MainPage(tk.Frame):

    def var_states(self):  # this is supposed to send code to run plot() again but it doesnt do it
        print(self.combobox.get())
        print(self.colr)
        self.plot()

    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)

        label = tk.Label(self, text="Graph Page!", font=LARGE_FONT)
        label.pack(pady=10, padx=10)

        values = ['jet', 'rainbow', 'ocean', 'hot',
                  'cubehelix', 'gnuplot', 'terrain', 'prism', 'pink']
        button1 = tk.Button(self, text="Back to Home",
                            command=lambda: controller.show_frame(StartPage))
        button1.pack()

        button2 = tk.Button(self, text="Re-Render",
                            command=self.plot)
        button2.pack()
        self.mvar = tk.IntVar()
        self.cbutton = tk.Checkbutton(
            self, text="shadow", onvalue=0, offvalue=1, variable=self.mvar)
        self.cbutton.pack()

        self.combobox = ttk.Combobox(self, values=values)
        self.combobox.current(0)
        self.combobox.pack(side=tk.TOP)

        self.numberInput = tk.Entry(self)
        self.numberInput.pack(side=tk.TOP)

        self.width, self.height = 10, 10
        fig = Figure(figsize=(self.width, self.height))
        self.ax = fig.add_subplot(111)

        self.canvas = FigureCanvasTkAgg(fig, self)
        self.canvas.draw()
        toolbar = NavigationToolbar2Tk(self.canvas, self)
        toolbar.update()
        self.canvas.get_tk_widget().pack(side=tk.BOTTOM, fill=tk.BOTH, expand=True)

        self.plot()

    def plot(self):
        colr = self.combobox.get()
        number=self.numberInput.get()
        print(number)
        self.ax.clear()
        # mandelbrot_image(self.ax, -0.8, -0.7, 0, 0.1, cmap=colr)
        curvefit(self.ax,number,number)
        self.canvas.draw()


app = base()
app.geometry("800x600")
app.mainloop()
