import ui_plot

import sys

from SwhRecorder import *

from PyQt4 import QtCore, QtGui

def updateData():
    print "updating data"
    if SHR.LOCK_COUNT<0:
        uiplot.label.setText("LOCKED")
    uiplot.textBrowser.setText(str(SHR.data))
    SHR.data=[]
    

if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    
    ### SET-UP recorder class
    SHR=SwhRecorder()
    SHR.SHRFP_decode=True
    SHR.setup()
    SHR.continuousStart()
    
    ### SET-UP WINDOWS
    
    # WINDOW plot
    win_plot = ui_plot.QtGui.QMainWindow()
    uiplot = ui_plot.Ui_win_plot()
    uiplot.setupUi(win_plot)
    
    uiplot.timer = QtCore.QTimer()
    uiplot.timer.start(100.0) #every 100ms
    uiplot.label.setText("NOT LOCKED")
    win_plot.connect(uiplot.timer, QtCore.SIGNAL('timeout()'), updateData) 

    ### DISPLAY WINDOWS
    win_plot.show()

    #WAIT UNTIL QT RETURNS EXIT CODE
    sys.exit(app.exec_())