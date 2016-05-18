import moose

def model(element):
    """Given model root element, extracts morphology
    information about all neurons and compartments belonging
    to that model
    """
    if element.path.endswith("/") :
        return morphology(moose.element(element.path + "cells"))
    else:
        return morphology(moose.element(element.path + "/cells"))

def morphology(element):
    """Given the element containing all neurons, extracts
    morphology information from all child elements.
    """
    return { "name"     : element.name
           , "id"       : element.path
           , "type"     : etype(element)
           , "neurons"  : recurse(element, neuron, moose.Neuron)
           }

def neuron(element):
    """Given a neuron element, extracts morphology
    information from all child compartment elements.
    """
    return element.path, { "name"            : element.name
                         , "id"              : element.path
                         , "object"          : element
                         , "type"            : etype(element)
                         , "compartments"    : recurse(element, compartment, moose.CompartmentBase)
                         }

def compartment(element):
    return element.path, { "name"       : element.name
                         , "object"     : element
                         , "id"         : element.path
                         , "type"       : etype(element)
                         , "proximal"   : { "x" : element.x0
                                          , "y" : element.y0
                                          , "z" : element.z0
                                          }
                         , "distal"     : { "x" : element.x
                                          , "y" : element.y
                                          , "z" : element.z
                                          }
                         , "diameter"   : element.diameter
                         }

def etype(element):
    element_type = element.name.rpartition("_")[0]
    if element_type == '': return element.name
    return element_type

def recurse(element, func, classType):
    return dict( [ func(child[0]) for child in element.children
                                  if isinstance(child, classType)
                 ]
               )
