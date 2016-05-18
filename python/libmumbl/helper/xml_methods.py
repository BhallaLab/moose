#!/usr/bin/env python

__author__  = "Dilawar Singh <dilawars@ncbs.res.in>"

__log__     = """

    Methods to help parsing xml.

"""
import re
import sys

def elemExists(rootElem, nmExpr, path) :
  elem = rootElem.xpath(nmExpr, name=path) 
  if elem :
    return True
  else :
    return False


def getElement(rootElem, nmlExpr, path) :
  elem = rootElem.xpath(nmlExpr, name=path) 
  return elem

def isTaggedWith(nmElem, tag) :
  regex = re.compile(r"\{.+\}"+tag+'$')
  m = regex.match(nmElem.tag)
  if m : return True
  else : return False

def getTagName(nmTag) :
  regex = re.compile(r"\{.+\}(?P<name>\w+)$")
  m = regex.match(nmTag)
  return m.group('name')

