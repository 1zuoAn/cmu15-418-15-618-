#!/usr/bin/python

# Visualize images of Mandelbrot set

from PIL import Image, ImageTk
import Tkinter

import os
import sys
import subprocess
import datetime

# Imported from
# http://code.activestate.com/recipes/577977-get-single-keypress/

# Function to get a single character
# Designed to work with Windows and Unix-like systems
try:
    import tty, termios
except ImportError:
    # Probably Windows.
    try:
        import msvcrt
    except ImportError:
        # FIXME what to do on other platforms?
        # Just give up here.
        raise ImportError('getch not available')
    else:
        getch = msvcrt.getch
else:
    def getch():
        """getch() -> key character

        Read a single keypress from stdin and return the resulting character. 
        Nothing is echoed to the console. This call will block if a keypress 
        is not already available, but will not wait for Enter to be pressed. 

        If the pressed key was a modifier key, nothing will be detected; if
        it were a special function key, it may return the first character of
        of an escape sequence, leaving additional characters in the buffer.
        """
        fd = sys.stdin.fileno()
        old_settings = termios.tcgetattr(fd)
        try:
            tty.setraw(fd)
            ch = sys.stdin.read(1)
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        return ch

class MandelImage:
    tk = None
    canvas = None
    currentImage = None
    # (x0, y0, x1, y1)
    homeField = [-2.167, -1.0, 1.167, 1]
    currentField = [0, 0, 0, 0] 
    width = 1250
    height = 750
    currentImage = None
    currentIndex = None
    zoomFactor = 1.0718 # 2^-10
    shiftFactor = 0.10
    commandList = []
    imageFile = 'mviz.ppm'

    def __init__(self, commandLine = ''):
        self.commandList = commandLine.split()
        self.tk = Tkinter.Tk()
        self.canvas = Tkinter.Canvas(self.tk, bg="black")
        self.canvas.config(width=self.width, height=self.height)
        self.canvas.pack(side=Tkinter.TOP)
        self.currentField = self.homeField
        self.help()

    def help(self):
        print "Keyboard commands:"
        print " + Zoom in"
        print " - Zoom out"
        print " l Shift left"
        print " r Shift right"
        print " u Shift up"
        print " d Shift down"
        print " h Return to original image"
        print " q Quit"


    def displayImage(self, image):
        saveIndex = self.currentIndex
        self.currentImage = ImageTk.PhotoImage(image)
        self.currentIndex = self.canvas.create_image((self.width/2, self.height/2), image = self.currentImage)
        if saveIndex is not None:
            self.canvas.delete(saveIndex)
        self.canvas.update()
    
    def displayFile(self, fname):
        im = Image.open(fname)
        self.displayImage(im)

    def timeFiles(self, flist, ntimes = 100):
        start = datetime.datetime.now()
        for idx in range(ntimes):
            fname = flist[idx % len(flist)]
            self.displayFile(fname)
        dt = datetime.datetime.now() - start
        tpi = dt/ntimes
        msecs = tpi.seconds/1000.0 + 1e-3*tpi.microseconds
        print "%.2f ms/image" % msecs

    def getImage(self):
        # Tell generator what we want:
        params = "%f:%f:%f:%f\n" % tuple(self.currentField)
        clist = self.commandList + ['-f', params, '-o', self.imageFile]
        try:
            p = subprocess.Popen(clist, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            (stdoutData, stderrData) = p.communicate()
        except Exception as ex:
            print "Couldn't run command '%s' (%s)" % (" ".join(clist), ex)
            return None
        try:
            im = Image.open(self.imageFile)
        except Exception as ex:
            print "Couldn't open image file '%s' (%s)" % (self.imageFile, ex)
            return None
        return im
        
    def zoomIn(self):
        deltax = self.currentField[2] - self.currentField[0]
        deltay = self.currentField[3] - self.currentField[1]
        cx = (self.currentField[2] + self.currentField[0])/2
        cy = (self.currentField[3] + self.currentField[1])/2
        deltax = deltax / self.zoomFactor
        deltay = deltay / self.zoomFactor
        x0 = cx - deltax/2
        y0 = cy - deltay/2
        x1 = cx + deltax/2
        y1 = cy + deltay/2
        self.currentField = (x0, y0, x1, y1)

    def zoomOut(self):
        deltax = self.currentField[2] - self.currentField[0]
        deltay = self.currentField[3] - self.currentField[1]
        cx = (self.currentField[2] + self.currentField[0])/2
        cy = (self.currentField[3] + self.currentField[1])/2
        deltax = deltax * self.zoomFactor
        deltay = deltay * self.zoomFactor
        x0 = cx - deltax/2
        y0 = cy - deltay/2
        x1 = cx + deltax/2
        y1 = cy + deltay/2
        self.currentField = (x0, y0, x1, y1)

    def shiftRight(self):
        x0 = self.currentField[0]
        y0 = self.currentField[1]
        x1 = self.currentField[2]
        y1 = self.currentField[3]
        deltax = x1 - x0
        x0 += deltax * self.shiftFactor
        x1 += deltax * self.shiftFactor
        self.currentField = (x0, y0, x1, y1)

    def shiftLeft(self):
        x0 = self.currentField[0]
        y0 = self.currentField[1]
        x1 = self.currentField[2]
        y1 = self.currentField[3]
        deltax = x1 - x0
        x0 -= deltax * self.shiftFactor
        x1 -= deltax * self.shiftFactor
        self.currentField = (x0, y0, x1, y1)

    def shiftUp(self):
        x0 = self.currentField[0]
        y0 = self.currentField[1]
        x1 = self.currentField[2]
        y1 = self.currentField[3]
        deltay = y1 - y0
        y0 -= deltay * self.shiftFactor
        y1 -= deltay * self.shiftFactor
        self.currentField = (x0, y0, x1, y1)

    def shiftDown(self):
        x0 = self.currentField[0]
        y0 = self.currentField[1]
        x1 = self.currentField[2]
        y1 = self.currentField[3]
        deltay = y1 - y0
        y0 += deltay * self.shiftFactor
        y1 += deltay * self.shiftFactor
        self.currentField = (x0, y0, x1, y1)
        
    def home(self):
        self.currentField = self.homeField

    def run(self):
        done = False
        while not done:
            im = self.getImage()
            if im is not None:
                self.displayImage(im)
            cmd = getch()
            try:
                os.remove(self.imageFile)
            except:
                pass
            if cmd == 'q':
                break
            elif cmd == '+':
                self.zoomIn()
            elif cmd == '-':
                self.zoomOut()
            elif cmd == 'u':
                self.shiftUp()
            elif cmd == 'd':
                self.shiftDown()
            elif cmd == 'l':
                self.shiftLeft()
            elif cmd == 'r':
                self.shiftRight()
            elif cmd == 'h':
                self.home()
            else:
                print "Unknown command '%s'" % cmd
        sys.exit(0)

def run(name, args):
    commandLine = " ".join(args)
    m = MandelImage(commandLine)
    m.run()


if __name__ == "__main__":
    run(sys.argv[0], sys.argv[1:])

                
            
