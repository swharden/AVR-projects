import ui_plot
import sys

from SwhRecorder2 import *

from PyQt4 import QtCore, QtGui
import PyQt4.Qwt5 as Qwt

def updateData():
    if SHR.nothingNewToShow:
        return
    else:
        SHR.nothingNewToShow=True
    
    # DATA WINDOW
    data=SHR.data
    uiplot.textBrowser.setText(str(data))
    
    # PLOT - DATA
    curveData.setData(numpy.arange(len(SHR.lastAudio)),SHR.lastAudio[:200])
    uiplot.qwtPlotSync.replot()
    
    # PLOT - SYNC
    hist, bin_edges = numpy.histogram(SHR.pulses,bins=numpy.arange(max(SHR.pulses)+5))
    curveSync.setData(bin_edges,hist)
    uiplot.qwtPlotData.replot()    
    
    # PLOT - DECODED DATA
    curveDecode.setData(range(len(SHR.data)),SHR.data)
    uiplot.qwtPlotData_2.replot()
    
    # TEXT
    uiplot.lbl_bitsPerSec.setText("%.02f"%(SHR.totalBitRate))
    uiplot.lbl_pointsPerSec.setText("%.02f"%(SHR.totalDataRate))
    

if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)

    ### SET-UP WINDOWS
    
    # WINDOW plot
    win_plot = ui_plot.QtGui.QMainWindow()
    uiplot = ui_plot.Ui_win_plot()
    uiplot.setupUi(win_plot)

    ### GET RECORDER GOING
    SHR=SwhRecorder()
    SHR.SHRFP_decode=True
    SHR.setup()
    SHR.continuousDataGo()
    
    curveSync=Qwt.QwtPlotCurve()  
    curveSync.attach(uiplot.qwtPlotSync)
    
    curveData=Qwt.QwtPlotCurve()  
    curveData.attach(uiplot.qwtPlotData)
    
    curveDecode=Qwt.QwtPlotCurve()  
    curveDecode.attach(uiplot.qwtPlotData_2)
    
    uiplot.timer = QtCore.QTimer()
    uiplot.timer.start(10.0)
    win_plot.connect(uiplot.timer, QtCore.SIGNAL('timeout()'), updateData) 

    ### DISPLAY WINDOWS
    win_plot.show()

    #WAIT UNTIL QT RETURNS EXIT CODE
    sys.exit(app.exec_())