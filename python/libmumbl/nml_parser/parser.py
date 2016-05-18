# Basic imports
import os
import sys
import logging
import debug.debug as debug
import inspect

logger = logging.getLogger('multiscale')
from lxml import etree

import collections
def parseAndValidateWithSchema(modelName, modelPath) :

    prefixPath = ''
    if modelName == 'xml' :
        schemaPath = os.path.join(prefixPath, 'schema/moose/moose.xsd')
        if not os.path.isfile(schemaPath) :
            debug.printDebug("WARN", "Schema {0} does not exists..".format(schemaPath))

    try :
        schemaH = open(schemaPath, "r")
        schemaText = schemaH.read()
        schemaH.close()
    except Exception as e :
        debug.printDebug("WARN", "Error reading schema for validation."+
          " Falling back to validation-disabled parser."
          + " Failed with error {0}".format(e))
        return parseWithoutValidation(modelName, modelPath)
        # Now we have the schema text
    schema = etree.XMLSchema(etree.XML(schemaText))
    xmlParser = etree.XMLParser(schema=schema, remove_comments=True)
    with open(modelPath, "r") as xmlTextFile :
        return etree.parse(xmlTextFile, xmlParser)

def parseWithoutValidation(modelName, modelPath) :
    xmlParser = etree.XMLParser(remove_comments=True)
    try :
        xmlRootElem = etree.parse(modelPath, xmlParser)
    except Exception as e :
        debug.printDebug("ERROR", "Parsing of {0} failed.".format(modelPath))
        debug.printDebug("DEBUG", "Error: {0}".format(e))
        raise RuntimeError, "Failed to parse XML"
    return xmlRootElem

def parseXMLs(commandLineArgs, validate=False) :
    xmlRootElemDict = collections.defaultdict(list)
    models = vars(commandLineArgs)
    for model in models :
        if models[model] :
            for modelPath in models[model] :
                debug.printDebug("INFO", "Parsing {0}".format(models[model]))
                if validate :
                    # parse model and valid it with schama
                    modelXMLRootElem = parseAndValidateWithSchema(model, modelPath)
                else :
                    # Simple parse the model without validating it with schema.
                    modelXMLRootElem = parseWithoutValidation(model, modelPath)
                if modelXMLRootElem :
                    xmlRootElemDict[model].append((modelXMLRootElem, modelPath))
    assert len(xmlRootElemDict) > 0
    return xmlRootElemDict

