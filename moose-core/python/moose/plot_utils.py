

"""plot_utils.py: Some utility function for plotting data in moose.

Last modified: Sun Jan 10, 2016  04:04PM

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, NCBS Bangalore"
__credits__          = ["NCBS Bangalore", "Bhalla Lab"]
__license__          = "GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import matplotlib.pyplot as plt
from . import _moose as moose
from . import print_utils as pu 
import numpy as np

def plotAscii(yvec, xvec = None, file=None):
    """Plot two list-like object in terminal using gnuplot.
    If file is given then save data to file as well.
    """
    if xvec is None:
        plotInTerminal(yvec, list(range( len(yvec))), file=file)
    else:
        plotInTerminal(yvec, xvec, file=file)

def plotInTerminal(yvec, xvec = None, file=None):
    '''
    Plot given vectors in terminal using gnuplot.

    If file is not None then write the data to a file.
    '''
    import subprocess
    g = subprocess.Popen(["gnuplot"], stdin=subprocess.PIPE)
    g.stdin.write("set term dumb 100 25\n")
    g.stdin.write("plot '-' using 1:2 title '{}' with linespoints\n".format(file))
    if file:
        saveAsGnuplot(yvec, xvec, file=file)
    for i,j in zip(xvec, yvec):
        g.stdin.write("%f %f\n" % (i, j))
    g.stdin.write("\n")
    g.stdin.flush()

def xyToString( yvec, xvec, sepby = ' '):
    """ Given two list-like objects, returns a text string. 
    """
    textLines = []
    for y, x in zip( yvec, xvec ):
        textLines.append("{}{}{}".format(y, sepby, x))
    return "\n".join(textLines)


def saveNumpyVec( yvec, xvec, file):
    """save the numpy vectors to a data-file
    
    """
    if file is None:
        return
    print(("[INFO] Saving plot data to file {}".format(file)))
    textLines = xyToString( yvec, xvec)
    with open(file, "w") as dataF:
        dataF.write(textLines)

def saveAsGnuplot( yvec, xvec, file):
    ''' Save the plot as stand-alone gnuplot script '''
    if file is None:
        return
    print(("[INFO] Saving plot data to a gnuplot-script: {}".format(file)))
    dataText = xyToString( yvec, xvec )
    text = []
    text.append("#!/bin/bash")
    text.append("gnuplot << EOF")
    text.append("set term post eps")
    text.append("set output \"{0}.eps\"".format(file))
    text.append("plot '-' using 0:1 title '{0}'".format(file))
    text.append(dataText)
    text.append("EOF")
    with open(file+".gnuplot","w") as gnuplotF:
        gnuplotF.write("\n".join(text))

def scaleVector(vec, scaleF):
    """ Scale a vector by a factor """
    if scaleF == 1.0 or scaleF is None:
        return vec
    else:
        return [ x*scaleF for x in vec ]

def scaleAxis(xvec, yvec, scaleX, scaleY):
    """ Multiply each elements by factor """
    xvec = scaleVector( xvec, scaleX )
    yvec = scaleVector( yvec, scaleY )
    return xvec, yvec

def reformatTable(table, kwargs):
    """ Given a table return x and y vectors with proper scaling """
    clock = moose.Clock('/clock')
    if type(table) == moose.Table:
        vecY = table.vector 
        vecX = np.arange(0, clock.currentTime, len(vecY))
    elif type(table) == tuple:
        vecX, vecY = table
    return (vecX, vecY)

def plotTable(table, **kwargs):
    """Plot a given table. It plots table.vector

    This function can scale the x-axis. By default, y-axis and x-axis scaling is
    done by a factor of 1.  

    Pass 'xscale' and/or 'yscale' argument to function to modify scales.
    
    """
    if not type(table) == moose.Table:
        msg = "Expected moose.Table, got {}".format( type(table) )
        raise TypeError(msg)

    vecX, vecY = reformatTable(table, kwargs)
    plt.plot(vecX, vecY, label = kwargs.get('label', ""))
    # This may not be available on older version of matplotlib.
    try:
        plt.legend(loc='best', framealpha=0.4)
    except:
        plt.legend(loc='best')

def plotTables(tables, outfile=None, **kwargs):
    """Plot a list of tables onto one figure only.
    """
    assert type(tables) == dict, "Expected a dict of moose.Table"
    plt.figure(figsize=(10, 1.5*len(tables)))
    subplot = kwargs.get('subplot', True)
    for i, tname in enumerate(tables):
        if subplot:
            plt.subplot(len(tables), 1, i+1)
        yvec = tables[tname].vector 
        xvec = np.linspace(0, moose.Clock('/clock').currentTime, len(yvec))
        plt.plot(xvec, yvec, label=tname)

        # This may not be available on older version of matplotlib.
        try:
            plt.legend(loc='best', framealpha=0.4)
        except:
            plt.legend(loc = 'best')
    
    plt.tight_layout()
    if outfile:
        pu.dump("PLOT", "Saving plots to file {}".format(outfile))
        try:
            plt.savefig(outfile, transparent=True)
        except Exception as e:
            pu.dump("WARN"
                    , "Failed to save figure, plotting onto a window"
                    )
            plt.show()
    else:
        plt.show()

def plotVector(vec, xvec = None, **options):
    """plotVector: Plot a given vector. On x-axis, plot the time.

    :param vec: Given vector.
    :param **kwargs: Optional to pass to maplotlib.
    """
    ax = options[ 'ax' ]
    assert type(vec) == np.ndarray, "Expected type %s" % type(vec)
    legend = options.get('legend', True)

    if xvec is None:
        clock = moose.Clock('/clock')
        xx = np.linspace(0, clock.currentTime, len(vec))
    else:
        xx = xvec[:]

    assert len(xx) == len(vec), "Expecting %s got %s" % (len(vec), len(xvec))
    ax.plot(xx, vec, label=options.get('label', ''))

    if legend:
        # This may not be available on older version of matplotlib.
        try:
            ax.legend(loc='best', framealpha=0.4)
        except:
            ax.legend(loc='best')

    if xvec is None:
        ax.set_xlabel('Time (sec)')
    else:
        ax.set_xlabel(options.get('xlabel', ''))
    
    ax.set_ylabel = options.get('ylabel', '')
    ax.set_title(options.get('title', ''))

    if(options.get('legend', True)):
        try:
            ax.legend(loc='best', framealpha=0.4, prop={'size' : 9})
        except:
            ax.legend(loc='best', prop={'size' : 9})
    return ax


def saveRecords(records, xvec = None, **kwargs):
    """saveRecords 
    Given a dictionary of data with (key, numpy array) pair, it saves them to a
    file 'outfile'

    :param outfile
    :param dataDict:
    :param **kwargs:
        comment: Adds comments below the header.
    """
    if len(records) == 0:
        pu.warn("No data in dictionary to save.")
        return False

    outfile = kwargs.get('outfile', 'data.moose')
    clock = moose.Clock('/clock')
    assert clock.currentTime > 0
    yvecs = [ ]
    text = "time," + ",".join([ str(x) for x in records ])
    for k in records:
        try:
            yvec = records[k].vector
        except AtrributeError as e:
            yevc = records[k]
        yvecs.append(yvec)
    xvec = np.linspace(0, clock.currentTime, len(yvecs[0]))
    yvecs = [ xvec ] + yvecs
    if kwargs.get('comment', ''):
        text += ("\n"  + kwargs['comment'] )
    np.savetxt(outfile, np.array(yvecs).T, delimiter=',' , header = text)
    pu.info("Done writing data to %s" % outfile)

def plotRecords(records, xvec = None, **kwargs):
    """plotRecords Plot given records in dictionary.

    :param records:
    :param xvec: If None, use moose.Clock to generate xvec.
    :param **kwargs:
    """
    dataDict = {}
    try:
        for k in sorted(records.keys(), key=str.lower):
            dataDict[k] = records[k]
    except Exception as e:
        dataDict = records

    legend = kwargs.get('legend', True)
    outfile = kwargs.get('outfile', None)
    subplot = kwargs.get('subplot', False)
    filters = [ x.lower() for x in kwargs.get('filter', [])]

    plt.figure(figsize=(10, 1.5*len(dataDict)))
    #plt.rcParams.update( { 'font-size' : 10 } )
    for i, k in enumerate(dataDict):
        pu.info("+ Plotting for %s" % k)
        plotThis = False
        if not filters: plotThis = True
        for accept in filters:
            if accept in k.lower(): 
                plotThis = True
                break
                
        if plotThis:
            if not subplot: 
                yvec = dataDict[k].vector
                plotVector(yvec, xvec, label=k, **kwargs)
            else:
                kwargs[ 'ax' ] = plt.subplot(len(dataDict), 1, i)
                yvec = dataDict[k].vector
                plotVector(yvec, xvec, label=k, **kwargs)

    # title in Image.
    if 'title' in kwargs:
        plt.title(kwargs['title'])

    if subplot:
        try:
            plt.tight_layout()
        except: pass

    if outfile:
        pu.info("Writing plot to %s" % outfile)
        plt.savefig("%s" % outfile, transparent=True)
    else:
        plt.show()

def plot_records( data_dict, xvec = None, **kwargs ):
    """Renamed (deprecated)
    """
    return plot_tables( data_dict, xvec, **kwargs )

def plot_tables(data_dict, xvec = None, **kwargs):
    """plot_tables plots moose.Table stored in a dictionary.

    :param data_dict:
    :param xvec: If None, use moose.Clock to generate xvec.
    :param **kwargs:
    """

    legend = kwargs.get('legend', True)
    outfile = kwargs.get('outfile', None)
    subplot = kwargs.get('subplot', False)
    filters = [ x.lower() for x in kwargs.get('filter', [])]

    ax = kwargs.get( 'ax', None )
    if ax is None:
        plt.figure(figsize=(10, 1.5*len(data_dict)))
        if not subplot:
            ax = plt.subplot( 1, 1, 1 )

    for i, k in enumerate(data_dict):
        pu.info("+ Plotting for %s" % k)
        plotThis = False
        if not filters: plotThis = True
        for accept in filters:
            if accept in k.lower(): 
                plotThis = True
                break
                
        if plotThis:
            if not subplot: 
                yvec = data_dict[k].vector
                plotVector(yvec, xvec, label=k, **kwargs)
            else:
                ax = plt.subplot(len(data_dict), 1, i)
                yvec = data_dict[k]
                plotVector(yvec, xvec, label=k, **kwargs)
    if subplot:
        try:
            plt.tight_layout()
        except: pass

    if outfile:
        pu.info("Writing plot to %s" % outfile)
        plt.savefig("%s" % outfile, transparent=True)
    else:
        plt.show()

