# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_plot.ui'
#
# Created: Tue May 14 23:49:20 2013
#      by: PyQt4 UI code generator 4.9.5
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_win_plot(object):
    def setupUi(self, win_plot):
        win_plot.setObjectName(_fromUtf8("win_plot"))
        win_plot.resize(643, 479)
        self.centralwidget = QtGui.QWidget(win_plot)
        self.centralwidget.setObjectName(_fromUtf8("centralwidget"))
        self.verticalLayout = QtGui.QVBoxLayout(self.centralwidget)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setMargin(6)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.qwtPlotSync = Qwt5.QwtPlot(self.centralwidget)
        self.qwtPlotSync.setObjectName(_fromUtf8("qwtPlotSync"))
        self.horizontalLayout.addWidget(self.qwtPlotSync)
        self.qwtPlotData = Qwt5.QwtPlot(self.centralwidget)
        self.qwtPlotData.setObjectName(_fromUtf8("qwtPlotData"))
        self.horizontalLayout.addWidget(self.qwtPlotData)
        self.verticalLayout.addLayout(self.horizontalLayout)
        self.horizontalLayout_2 = QtGui.QHBoxLayout()
        self.horizontalLayout_2.setContentsMargins(6, 0, 6, 0)
        self.horizontalLayout_2.setObjectName(_fromUtf8("horizontalLayout_2"))
        self.label_9 = QtGui.QLabel(self.centralwidget)
        self.label_9.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_9.setObjectName(_fromUtf8("label_9"))
        self.horizontalLayout_2.addWidget(self.label_9)
        self.lbl_bitsPerSec = QtGui.QLabel(self.centralwidget)
        self.lbl_bitsPerSec.setObjectName(_fromUtf8("lbl_bitsPerSec"))
        self.horizontalLayout_2.addWidget(self.lbl_bitsPerSec)
        self.label_13 = QtGui.QLabel(self.centralwidget)
        self.label_13.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_13.setObjectName(_fromUtf8("label_13"))
        self.horizontalLayout_2.addWidget(self.label_13)
        self.lbl_pointsPerSec = QtGui.QLabel(self.centralwidget)
        self.lbl_pointsPerSec.setObjectName(_fromUtf8("lbl_pointsPerSec"))
        self.horizontalLayout_2.addWidget(self.lbl_pointsPerSec)
        self.verticalLayout.addLayout(self.horizontalLayout_2)
        self.horizontalLayout_3 = QtGui.QHBoxLayout()
        self.horizontalLayout_3.setMargin(6)
        self.horizontalLayout_3.setObjectName(_fromUtf8("horizontalLayout_3"))
        self.textBrowser = QtGui.QTextBrowser(self.centralwidget)
        self.textBrowser.setObjectName(_fromUtf8("textBrowser"))
        self.horizontalLayout_3.addWidget(self.textBrowser)
        self.qwtPlotData_2 = Qwt5.QwtPlot(self.centralwidget)
        self.qwtPlotData_2.setObjectName(_fromUtf8("qwtPlotData_2"))
        self.horizontalLayout_3.addWidget(self.qwtPlotData_2)
        self.verticalLayout.addLayout(self.horizontalLayout_3)
        win_plot.setCentralWidget(self.centralwidget)

        self.retranslateUi(win_plot)
        QtCore.QMetaObject.connectSlotsByName(win_plot)

    def retranslateUi(self, win_plot):
        win_plot.setWindowTitle(QtGui.QApplication.translate("win_plot", "MainWindow", None, QtGui.QApplication.UnicodeUTF8))
        self.label_9.setText(QtGui.QApplication.translate("win_plot", "bits per second:", None, QtGui.QApplication.UnicodeUTF8))
        self.lbl_bitsPerSec.setText(QtGui.QApplication.translate("win_plot", "1234", None, QtGui.QApplication.UnicodeUTF8))
        self.label_13.setText(QtGui.QApplication.translate("win_plot", "points per second:", None, QtGui.QApplication.UnicodeUTF8))
        self.lbl_pointsPerSec.setText(QtGui.QApplication.translate("win_plot", "1234", None, QtGui.QApplication.UnicodeUTF8))
        self.textBrowser.setHtml(QtGui.QApplication.translate("win_plot", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:\'MS Shell Dlg 2\'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">RF DATA</span></p></body></html>", None, QtGui.QApplication.UnicodeUTF8))

from PyQt4 import Qwt5

if __name__ == "__main__":
    import sys
    app = QtGui.QApplication(sys.argv)
    win_plot = QtGui.QMainWindow()
    ui = Ui_win_plot()
    ui.setupUi(win_plot)
    win_plot.show()
    sys.exit(app.exec_())

