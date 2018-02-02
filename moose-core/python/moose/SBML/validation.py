# -*- coding: utf-8 -*-
'''
*******************************************************************
 * File:            validation.py
 * Description:
 * Author:          HarshaRani
 * E-mail:          hrani@ncbs.res.in
 ********************************************************************/

/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2017 Upinder S. Bhalla. and NCBS
Created : Thu May 12 10:19:00 2016(+0530)
Version
Last-Updated: Fri Jul 28 15:50:00 2017(+0530)
          By:
**********************************************************************/

'''
foundLibSBML_ = False
try:
    from libsbml import *
    foundLibSBML_ = True
except Exception as e:
    pass

def validateModel(sbmlDoc):
    if sbmlDoc.getNumErrors() > 0:
        tobecontinued = False
        for i in range(0,sbmlDoc.getNumErrors()):
            print (sbmlDoc.getError(i).getMessage())
            return False

    if (not sbmlDoc):
        print("validateModel: given a null SBML Document")
        return False

    consistencyMessages = ""
    validationMessages = ""
    noProblems = True
    numCheckFailures = 0
    numConsistencyErrors = 0
    numConsistencyWarnings = 0
    numValidationErrors = 0
    numValidationWarnings = 0
    # Once the whole model is done and before it gets written out,
    # it's important to check that the whole model is in fact complete,
    # consistent and valid.
    numCheckFailures = sbmlDoc.checkInternalConsistency()
    if (numCheckFailures > 0):
        for i in range(0, numCheckFailures):
            sbmlErr = sbmlDoc.getError(i)
            if (sbmlErr.isFatal() or sbmlErr.isError()):
                noProblems = False
                numConsistencyErrors += 1
            else:
                numConsistencyWarnings += 1
        constStr = sbmlDoc.printErrors()
        if sbmlDoc.printErrors():
            consistencyMessages = constStr

    # If the internal checks fail, it makes little sense to attempt
    # further validation, because the model may be too compromised to
    # be properly interpreted.

    if (numConsistencyErrors > 0):
        consistencyMessages += "Further validation aborted."
    else:
        numCheckFailures = sbmlDoc.checkConsistency()
        validationMessages;
        #numCheckFailures = sbmlDoc.checkL3v1Compatibility()
        if (numCheckFailures > 0):
            for i in range(0, (numCheckFailures)):
                consistencyMessages = sbmlDoc.getErrorLog().toString()
                sbmlErr = sbmlDoc.getError(i)
                if (sbmlErr.isFatal() or sbmlErr.isError()):
                    noProblems = False
                    numValidationErrors += 1
                else:
                    numValidationWarnings += 1

        warning = sbmlDoc.getErrorLog().toString()
        oss = sbmlDoc.printErrors()
        validationMessages = oss

    if (noProblems):
        return True
    else:
        if consistencyMessages is None:
            consistencyMessages = ""
        if consistencyMessages != "":
            print("consistency Warning: " + consistencyMessages)

        if (numConsistencyErrors > 0):
            print("ERROR: encountered " +str(numConsistencyErrors) +" consistency error in model " +sbmlDoc.getModel().getId() +"'.")

    if (numConsistencyWarnings > 0):
        print("Notice: encountered " +str(numConsistencyWarnings) +" consistency warning in model " +sbmlDoc.getModel().getId() +"'.")

    if (numValidationErrors > 0):
        print("ERROR: encountered " + str(numValidationErrors) + " validation error in model " +sbmlDoc.getModel().getId() + "'.")
        if (numValidationWarnings > 0):
            print("Notice: encountered " +str(numValidationWarnings) +" validation warning in model " +sbmlDoc.getModel().getId() +"'.")

    if validationMessages:
        print(validationMessages)

    return False
    # return ( numConsistencyErrors == 0 and numValidationErrors == 0,
    # consistencyMessages)

if __name__ == '__main__':
    sbmlDoc = libsbml.readSBML('00001-sbml-l3v1.xml')
    validateModel(sbmlDoc)
