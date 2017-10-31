# -*- python -*-
#
#       OpenAlea.Visualea: OpenAlea graphical user interface
#
#       Copyright 2006 INRIA - CIRAD - INRA  
#
#       File author(s): Samuel Dufour-Kowalski <samuel.dufour@sophia.inria.fr>
#                       Christophe Pradal <christophe.prada@cirad.fr>
#
#       Distributed under the CeCILL v2 License.
#       See accompanying file LICENSE.txt or copy at
#           http://www.cecill.info/licences/Licence_CeCILL_V2-en.html
# 
#       OpenAlea WebSite : http://openalea.gforge.inria.fr
#
# Fri Mar 15 15:10:58 IST 2013: This file was obtained from:
# http://obswww.unige.ch/~revaz/git/old.glups-4.0/glups/shell.py -
# Subhasis Ray

__doc__="""
This module implements a QT4 python interpreter widget.
It is inspired bu PyCute : http://gerard.vermeulen.free.fr
"""

__license__= "CeCILL V2"
__revision__=" $Id: shell.py 579 2007-06-13 10:27:57Z dufourko $"


import os, sys
from PyQt4 import QtCore, QtGui
from PyQt4.QtGui import QTextEdit, QTextCursor
from PyQt4.QtCore import Qt


class History:
  """ Backup and restore history between sessions 
      Class added to initial files. Only 3 lines modified to original files.
      Look for "Added by vb" comments
      Beware: The backup file will grow without limit. You must clean it manually.
  """
  def __init__(self, file):
    if os.path.isfile(file): # Le fichier existe
      self.f = open(file, 'r+')
      self.history = self.f.readlines()
    else: # Le fichier existe pas
      self.f = open(file, 'w')
      self.history = []
      
  def load(self, trunk = None):
      r = [str[:-1] for str in self.history]
      if not trunk:
        return r
      else: 
        return r[-trunk:]
  
  def append(self, word):
    self.history.append(word)
    self.f.write(word + '\n')
    self.f.flush()
      


def get_shell_class():
    """ Return the shell class to instantiate """

    # Test QScintilla
    try:
        from scishell import SciShell
        return SciShell
    
    except ImportError as e:
        return PyCutExt
    


class MultipleRedirection:
    """ Dummy file which redirects stream to multiple file """

    def __init__(self, files):
        """ The stream is redirect to the file list 'files' """

        self.files = files

    def write(self, str):
        """ Emulate write function """

        for f in self.files:
            f.write(str)
            

class PyCutExt(QTextEdit):

    """
    PyCute is a Python shell for PyQt.

    Creating, displaying and controlling PyQt widgets from the Python command
    line interpreter is very hard, if not, impossible.  PyCute solves this
    problem by interfacing the Python interpreter to a PyQt widget.

    """
    
    def __init__(self, interpreter, message="", log='', parent=None):
        """Constructor.
        @param interpreter : InteractiveInterpreter in which
        the code will be executed

        @param message : welcome message string
        
        @param 'log' : specifies the file in which the
        interpreter session is to be logged.

        @param  'parent' : specifies the parent widget.
        If no parent widget has been specified, it is possible to
        exit the interpreter by Ctrl-D.
        """

        QTextEdit.__init__(self, parent)
        self.interpreter = interpreter
        self.colorizer = SyntaxColor()

        # session log
        self.log = log or ''

        # to exit the main interpreter by a Ctrl-D if PyCute has no parent
        if parent is None:
            self.eofKey = Qt.Key_D
        else:
            self.eofKey = None

        # capture all interactive input/output 
        sys.stdout   = self
        sys.stderr   = MultipleRedirection((sys.stderr, self))
        sys.stdin    = self

        
        # last line + last incomplete lines
        self.line    = QtCore.QString()
        self.lines   = []
        # the cursor position in the last line
        self.point   = 0
        # flag: the interpreter needs more input to run the last lines. 
        self.more    = 0
        # flag: readline() is being used for e.g. raw_input() and input()
        self.reading = 0
        # history
        self.maxHistoryEntries = 1000                # Added by yr
        self.H = History("%s/.ghist"%os.environ['HOME'])     # Added by yr
        self.history = self.H.load(self.maxHistoryEntries)     # Added by yr    
        #self.history = []
        self.pointer = 0
        self.cursor_pos   = 0

        # user interface setup
        #self.setTextFormat(Qt.PlainText)
        self.setLineWrapMode(QTextEdit.NoWrap)
        #self.setCaption('Python Shell')

#         # font
#         if os.name == 'posix':
#             font = QtGui.QFont("Fixed", 8)
#         elif os.name == 'nt' or os.name == 'dos':
#             font = QtGui.QFont("Courier New", 8)
#         else:
#             raise SystemExit, "FIXME for 'os2', 'mac', 'ce' or 'riscos'"
#         font.setFixedPitch(1)
#         self.setFont(font)

#         # geometry
#         height = 40*QtGui.QFontMetrics(font).lineSpacing()
#         request = QtCore.QSize(600, height)
#         if parent is not None:
#             request = request.boundedTo(parent.size())
#         self.resize(request)

        # interpreter prompt.
        try:
            sys.ps1
        except AttributeError as e:
            sys.ps1 = ">>> "
        try:
            sys.ps2
        except AttributeError as e:
            sys.ps2 = "... "

        # interpreter banner
        self.write('The shell running Python %s on %s.\n' %
                   (sys.version, sys.platform))
        self.write('Type "copyright", "credits" or "license"'
                   ' for more information on Python.\n')
        self.write(message+'\n\n')
        self.write('This is the standard Shell.\n'+
                   'Autocompletion is not available unless QScintilla is installed:\n'+
                   'See http://www.riverbankcomputing.co.uk/qscintilla.\n\n')
        self.write(sys.ps1)
        

    def get_interpreter(self):
        """ Return the interpreter object """

        return self.interpreter
        

    def moveCursor(self, operation, mode=QTextCursor.MoveAnchor):
        """
        Convenience function to move the cursor
        This function will be present in PyQT4.2
        """
        cursor = self.textCursor()
        cursor.movePosition(operation, mode)
        self.setTextCursor(cursor)
        

    def flush(self):
        """
        Simulate stdin, stdout, and stderr.
        """
        pass


    def isatty(self):
        """
        Simulate stdin, stdout, and stderr.
        """
        return 1
    

    def readline(self):
        """
        Simulate stdin, stdout, and stderr.
        """
        self.reading = 1
        self.__clearLine()
        self.moveCursor(QTextCursor.End)
        while self.reading:
            qApp.processOneEvent()
        if self.line.length() == 0:
            return '\n'
        else:
            return str(self.line) 

    
    def write(self, text):
        """
        Simulate stdin, stdout, and stderr.
        """
        # The output of self.append(text) contains to many newline characters,
        # so work around QTextEdit's policy for handling newline characters.

        cursor = self.textCursor()

        cursor.movePosition(QTextCursor.End)

        pos1 = cursor.position()
        cursor.insertText(text)

        self.cursor_pos = cursor.position()
        self.setTextCursor(cursor)
        self.ensureCursorVisible ()

        # Set the format
        cursor.setPosition(pos1, QTextCursor.KeepAnchor)
        format = cursor.charFormat()
        format.setForeground( QtGui.QBrush(QtGui.QColor(0,0,0)))
        cursor.setCharFormat(format)


    def writelines(self, text):
        """
        Simulate stdin, stdout, and stderr.
        """
        map(self.write, text)


    def fakeUser(self, lines):
        """
        Simulate a user: lines is a sequence of strings, (Python statements).
        """
        for line in lines:
            self.line = QtCore.QString(line.rstrip())
            self.write(self.line)
            self.write('\n')
            self.__run()

            
    def __run(self):
        """
        Append the last line to the history list, let the interpreter execute
        the last line(s), and clean up accounting for the interpreter results:
        (1) the interpreter succeeds
        (2) the interpreter fails, finds no errors and wants more line(s)
        (3) the interpreter fails, finds errors and writes them to sys.stderr
        """
        self.pointer = 0
        self.history.append(QtCore.QString(self.line))
        self.H.append(QtCore.QString(self.line)) # Added by yr
        try:
            self.lines.append(str(self.line))
        except Exception as e:
            print( e )

        source = '\n'.join(self.lines)
        self.more = self.interpreter.runsource(source)

        if self.more:
            self.write(sys.ps2)
        else:
            self.write(sys.ps1)
            self.lines = []
        self.__clearLine()

        
    def __clearLine(self):
        """
        Clear input line buffer
        """
        self.line.truncate(0)
        self.point = 0

        
    def __insertText(self, text):
        """
        Insert text at the current cursor position.
        """
        self.line.insert(self.point, text)
        #self.point += text.length()
        self.point += len(text)                 #Added by yr

        cursor = self.textCursor()
        cursor.insertText(text)
        self.color_line()


    def keyPressEvent(self, e):
        """
        Handle user input a key at a time.
        """
        text  = e.text()
        key   = e.key()

        if key == Qt.Key_Backspace:
            if self.point:
                cursor = self.textCursor()
                cursor.movePosition(QTextCursor.PreviousCharacter, QTextCursor.KeepAnchor)
                cursor.removeSelectedText()
                self.color_line()
            
                self.point -= 1 
                self.line.remove(self.point, 1)

        elif key == Qt.Key_Delete:
            cursor = self.textCursor()
            cursor.movePosition(QTextCursor.NextCharacter, QTextCursor.KeepAnchor)
            cursor.removeSelectedText()
            self.color_line()
                        
            self.line.remove(self.point, 1)
            
        elif key == Qt.Key_Return or key == Qt.Key_Enter:
            self.write('\n')
            if self.reading:
                self.reading = 0
            else:
                self.__run()
                
        elif key == Qt.Key_Tab:
            self.__insertText(text)
        elif key == Qt.Key_Left:
            if self.point : 
                self.moveCursor(QTextCursor.Left)
                self.point -= 1 
        elif key == Qt.Key_Right:
            if self.point < self.line.length():
                self.moveCursor(QTextCursor.Right)
                self.point += 1 

        elif key == Qt.Key_Home:
            cursor = self.textCursor ()
            cursor.setPosition(self.cursor_pos)
            self.setTextCursor (cursor)
            self.point = 0 

        elif key == Qt.Key_End:
            self.moveCursor(QTextCursor.EndOfLine)
            self.point = self.line.length() 

        elif key == Qt.Key_Up:

            if len(self.history):
                if self.pointer == 0:
                    self.pointer = len(self.history)
                self.pointer -= 1
                self.__recall()
                
        elif key == Qt.Key_Down:
            if len(self.history):
                self.pointer += 1
                if self.pointer == len(self.history):
                    self.pointer = 0
                self.__recall()

        elif text.length():
            self.__insertText(text)
            return

        else:
            e.ignore()


    def __recall(self):
        """
        Display the current item from the command history.
        """
        cursor = self.textCursor ()
        cursor.select( QtGui.QTextCursor.LineUnderCursor )
        cursor.removeSelectedText()

        if self.more:
            self.write(sys.ps2)
        else:
            self.write(sys.ps1)
            

        self.__clearLine()
        self.__insertText(self.history[self.pointer])

        
#     def focusNextPrevChild(self, next):
#         """
#         Suppress tabbing to the next window in multi-line commands. 
#         """
#         if next and self.more:
#             return 0
#         return QTextEdit.focusNextPrevChild(self, next)

    def mousePressEvent(self, e):
        """
        Keep the cursor after the last prompt.
        """
        if e.button() == Qt.LeftButton:
            self.moveCursor(QTextCursor.End)
            

    def contentsContextMenuEvent(self,ev):
        """
        Suppress the right button context menu.
        """
        pass

    
    def color_line(self):
        """ Color the current line """
        
        cursor = self.textCursor()
        cursor.movePosition(QTextCursor.StartOfLine)

        newpos = cursor.position()
        pos = -1
        
        while(newpos != pos):
            cursor.movePosition(QTextCursor.NextWord)

            pos = newpos
            newpos = cursor.position()

            cursor.select(QTextCursor.WordUnderCursor)
            word = str(cursor.selectedText ().toAscii())

            if(not word) : continue
            
            (R,G,B) = self.colorizer.get_color(word)
            
            format = cursor.charFormat()
            format.setForeground( QtGui.QBrush(QtGui.QColor(R,G,B)))
            cursor.setCharFormat(format)
            




class SyntaxColor:
    """ Allow to color python keywords """

    keywords = set(["and", "del", "from", "not", "while",
                "as", "elif", "global", "or", "with",
                "assert", "else", "if", "pass", "yield",
                "break", "except", "import", "print",
                "class", "exec", "in", "raise",              
                "continue", "finally", "is", "return",
                "def", "for", "lambda", "try"])

    def __init__(self):
        pass
        

    def get_color(self, word):
        """ Return a color tuple (R,G,B) depending of the string word """

        stripped = word.strip()
        
        if(stripped in self.keywords):
            return (255, 132,0) # orange
        
        elif(self.is_python_string(stripped)):
            return (61, 120, 9) # dark green
        
        else:
            return (0,0,0)

    def is_python_string(self, str):
        """ Return True if str is enclosed by a string mark """

#         return (
#             (str.startswith("'''") and str.endswith("'''")) or
#             (str.startswith('"""') and str.endswith('"""')) or
#             (str.startswith("'") and str.endswith("'")) or
#             (str.startswith('"') and str.endswith('"')) 
#             )
        return False
        
        



