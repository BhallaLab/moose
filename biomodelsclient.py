# biomodelsclient.py --- 
# 
# Filename: biomodelsclient.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Tue Mar  2 07:57:39 2010 (+0530)
# Version: 
# Last-Updated: Wed Dec  11 15:47:32 2010 (+0530)
#           By: 
#     Update #: 
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# This is a client for Biomodels database SOAP service.
# It imitates the SOAP client written in JAVA
# availabele at biomodels website.

# Change log:
# 
# 
# 
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 3, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth
# Floor, Boston, MA 02110-1301, USA.
# 
# 

# Code:


from suds.client import Client
from suds.transport.http import HttpTransport as SudsHttpTransport
import os
import config
import pickle
import moose

BIOMODELS_WSDL_URI = 'http://www.ebi.ac.uk/biomodels-main/services/BioModelsWebServices?wsdl'
proxyOpts = dict()

for k,v in {'http':'http_proxy','https':'https_proxy'}.items():
    if os.environ.has_key(v):
        httpProxy = os.environ[v].replace('http://', '')
        proxyOpts[k] = httpProxy[0:httpProxy.rfind('/',0,len(httpProxy))]
    elif os.environ.has_key(v.upper()):
        HttpProxy = os.environ[v.upper()].replace('http://', '')
        proxyOpts[k] = HttpProxy[0:HttpProxy.rfind('/',0,len(HttpProxy))]

'''
class HttpTransport(SudsHttpTransport):
    """HttpTransport which properly obeys the ``*_proxy`` environment variables."""
    def u2handlers(self):
	return []
'''
class BioModelsClient(Client):
    def __init__(self, WSDL_URI=BIOMODELS_WSDL_URI):
	"""Initialize the client with the available queries listed in
	the WSDL file. All the queries can be executed using the following syntax:
	
	client.service.queryToBeExecuted()
	"""
	try:
	    Client.__init__(self, WSDL_URI,proxy=proxyOpts)
	    #Client.__init__(self, WSDL_URI,transport=HttpTransport())
	except Exception, e:
	    print e

from PyQt4.Qt import Qt
from PyQt4 import QtCore, QtGui

class BioModelsClientWidget(QtGui.QDialog):
    """This is a widget with a Biomodels Client. It provides simple
    access to the biomodel queries and gives the user a view of the
    results"""
    COMBO_ITEM_QUERY_MAP = [('All Curated Model Ids', 'getAllCuratedModelsId'),
                            ('All Model Ids', 'getAllModelsId'),
                            ('All Non-curated Model Ids', 'getAllNonCuratedModelsId'),
                            ('Model Name By Id', 'getModelNameById'),
                            ('Model Ids by ChEBI', 'getModelsIdByChEBI'),
                            ('Model Ids by ChEBI Id', 'getModelsIdByChEBIId'),
                            ('Model Ids by Gene Ontology Term', 'getModelsIdByGO'),
                            ('Model Ids by Gene Ontology Id', 'getModelsIdByGOId'),
                            ('Model Ids by Name', 'getModelsIdByName'),
                            ('Model Ids by Author/Modeler', 'getModelsIdByPerson'),
                            ('Model Ids by Publication', 'getModelsIdByPublication'),
                            ('Model Ids by Taxonomy', 'getModelsIdByTaxonomy'),]
    def __init__(self, parent=None):
	QtGui.QWidget.__init__(self, parent)
        self.setWindowTitle('Connect to BioModels')
	self.client = BioModelsClient()
        self.queryPanel = QtGui.QWidget(self)
        self.queryModelLabel = QtGui.QLabel('Get ', self.queryPanel)
        self.queryModelCombo = QtGui.QComboBox(self.queryPanel)
        self.queryLineEdit = QtGui.QLineEdit(self.queryPanel)
        self.goButton = QtGui.QPushButton('Go', self.queryPanel)
        self.filePath = ''
        layout = QtGui.QHBoxLayout(self.queryPanel)
        layout.addWidget(self.queryModelLabel)
        layout.addWidget(self.queryModelCombo)
        layout.addWidget(self.queryLineEdit)
        layout.addWidget(self.goButton)
        self.queryPanel.setLayout(layout)
        
        for entry in BioModelsClientWidget.COMBO_ITEM_QUERY_MAP:
            self.queryModelCombo.addItem(self.tr(entry[0]), QtCore.QVariant(entry[1]))
        self.resultsPanel = QtGui.QTableWidget(self)
        layout = QtGui.QVBoxLayout(self)
        layout.addWidget(self.queryPanel)
        layout.addWidget(self.resultsPanel)
        
        self.queryPanel1 = QtGui.QWidget(self)
        self.importButton = QtGui.QPushButton('Import',self.queryPanel1)
        self.importButton.setEnabled(False)
        self.closeButton = QtGui.QPushButton('Close',self.queryPanel1)
        hbox = QtGui.QHBoxLayout(self.queryPanel1)
        hbox.addStretch(1)
        hbox.addWidget(self.importButton)
        hbox.addWidget(self.closeButton)
        layout.addWidget(self.queryPanel1)

        self.setLayout(layout)
        self.setupActions()
        self.client.set_options(proxy=proxyOpts)
        
	# TODO:
	# proxy = [ can be set using set_option(proxy={'http':'proxyhost:port', ...}) function

    def setupActions(self):
        self.connect(self.queryLineEdit, QtCore.SIGNAL('returnPressed()'), self.runQuery)
        self.connect(self.goButton, QtCore.SIGNAL('clicked()'), self.runQuery)
        self.connect(self.closeButton, QtCore.SIGNAL('clicked()'),self.close)
        self.connect(self.importButton,QtCore.SIGNAL('clicked()'),self.downloadModel)
        self.connect(self.resultsPanel, QtCore.SIGNAL('cellClicked(int,int)'),self.enableimportButton)
    def closeWidget(self):
        self.close()
    
    def downloadModel(self):
        """Download the selected model"""
        """ If user select multi row, only data from currentRow is downloaded and loaded into moose """
        selectedRow = self.resultsPanel.currentRow()
        modelId = self.resultsPanel.item(selectedRow, 0).text()
        modelSBML = unicode(self.client.service.getModelSBMLById(modelId)).encode("utf-8")
        self.filePath = os.path.join(config.settings[config.KEY_LOCAL_DEMOS_DIR], str(modelId)+'.xml')
        f = open(str(self.filePath), 'w')
        f.write(modelSBML)
        self.close()
       
        type_sbml = 'SBML'
        filters = {'SBML(*.xml)': type_sbml}
        filepath,filter_ = QtGui.QFileDialog.getSaveFileNameAndFilter(None,'Save File',modelId,';;'.join(filters))
        if filepath:
            if str(filepath).rfind('.') != -1:
                filepath = filepath[:str(filepath).rfind('.')]
            if str(filter_).rfind('.') != -1:
                extension = filter_[str(filter_).rfind('.'):len(filter_)-1]
            self.filePath = str(filepath+extension)

            if filters[str(filter_)] == 'SBML':
                f = open(str(self.filePath), 'w')
                f.write(modelSBML)
                f.close()
         
    def getTargetPath(self):
        return str(self.filePath)

    def runQuery(self):
        print 'Running query .....'
        #self.resultsPanel.cellClicked.connect(self.enableDownload)
        progressDialog = QtGui.QProgressDialog()
        progressDialog.setLabelText('Retrieving data from BioModels Database')
        progressDialog.setModal(True)
        progressDialog.setVisible(True)
        progressDialog.setWindowTitle("BioModels Database")
        index = self.queryModelCombo.currentIndex()
        query = self.queryModelCombo.itemData(index).toString()
        argument = self.queryLineEdit.text().trimmed()
        function = eval('self.client.service.' + str(query))
        if index > 2:
            result = function(str(argument))
        else:
            result = function()

        self.resultsPanel.clear()

        row = 0
        column = 0
        self.resultsPanel.setColumnCount(2)
        self.resultsPanel.setHorizontalHeaderItem(column, QtGui.QTableWidgetItem('Id'))
        self.resultsPanel.setHorizontalHeaderItem(column + 1, QtGui.QTableWidgetItem('Name'))
        self.resultsPanel.setRowCount(0)
        display = True
        totalRows = 0
        if type(result) is type(''):
            self.resultsPanel.insertRow(row)
            item = QtGui.QTableWidgetItem(argument)
            item.setFlags(item.flags() & ~Qt.ItemIsEditable)
            self.resultsPanel.setItem(row, column, item)
            item = QtGui.QTableWidgetItem(result)
            item.setFlags(item.flags() & ~Qt.ItemIsEditable)
            self.resultsPanel.setItem(row, column + 1, item)
        elif type(result) is type([]):
            totalRows = len(result)
            updatepickleFile = False
            filename = str(query)
            filePath = os.path.join(config.settings[config.KEY_BIOMODEL_DIR], filename+'.pkl')
            #print filePath
            pickleResult = {}
            if os.path.isfile(filePath):
                pickleFile = open(filePath,'rb')
                pickleResult = pickle.load(pickleFile)

            #If there is update from BioModels,then pickle file is updated.
            r = 0
            if progressDialog:
                progressDialog.setMaximum(totalRows)

            for item in result:
                if item not in pickleResult:
                    if progressDialog:
                        progressDialog.setValue(r)            
                        if progressDialog.wasCanceled():
                            return 0
                        name = unicode(self.client.service.getModelNameById(item)).encode("utf-8")
                        r = r+1
                    pickleResult.update({item:name})
                    updatepickleFile = True
            if updatepickleFile:
                output = open(os.path.join(config.settings[config.KEY_BIOMODEL_DIR], filename+'.pkl'),'wb')
                pickle.dump(pickleResult,output)
                output.close()
                

            if not argument.isEmpty():
                try:
                    name = pickleResult[str(argument)]
                    pickleResult = {}
                    pickleResult[argument] = name;

                except KeyError, e:
                    print 'A KeyError - "%s"' % str(e) ,' not found in ',filename
                    QtGui.QMessageBox.critical(None, "BioModels Database"," The Id "+ str(e) +" not found in "+ filename,QtGui.QMessageBox.Ok | QtGui.QMessageBox.Default,QtGui.QMessageBox.NoButton)
                    display = False
            if display:
                for value,name in pickleResult.items():
                    self.resultsPanel.insertRow(row)
                    item = QtGui.QTableWidgetItem(self.tr(value))
                    item.setFlags(item.flags() & ~Qt.ItemIsEditable)
                    #item.setData(Qt.DisplayRole, QtCore.QVariant(value))
                    self.resultsPanel.setItem(row, column, item)
                    #name = self.client.service.getModelNameById(value)
                    item = QtGui.QTableWidgetItem(name)
                    item.setFlags(item.flags() & ~Qt.ItemIsEditable)
                    self.resultsPanel.setItem(row, column + 1, item)
                    row = row + 1
                    
        if progressDialog:
            progressDialog.close()
        #self.importButton.setEnabled(True)
        print 'Finished running query'

    def enableimportButton(self):
        self.importButton.setEnabled(True)
    
if __name__ == '__main__':
    client = BioModelsClient()
    #print dir(client)
    #print client.helloBioModels()

    app = QtGui.QApplication([])
    clientWidget = BioModelsClientWidget()
    #clientWidget.exec_()
    app.exec_()

# 
# biomodelsclient.py ends here
