#!/usr/bin/env python

__author__  = "Dilawar Singh <dilawars@ncbs.res.in>"

__log__     = """


"""

def segGrp2Color(grpName, grps):
    """
    Convert a  segmenet group name to an appropriate color.
    """
    index = 1
    for i in grps:
        if i != grpName:
            index += 1
        else:
            break
    level = format((index * 16777215)/len(grps), '06x')
    return "color=\"#"+level+"\""

def attachToNodesInGroup(cellG, groups, pName, pValue):
    """
    Attach a key=vlaue pair to node; these nodes belong to certain groups.
    """
    for g in groups:
        for n in cellG.nodes():
            if g in cellG.node[n]['belongsTo']:
                cellG.node[n][pName] = pValue


def write_dot(g, filename):
    """
    Write this graph to a graphviz file.
    """
    dotFile = []
    dotFile.append("digraph moose {")
    dotFile.append("dim=3")
    #dotFile.append("concentrate=True")
    if g.number_of_nodes() > 100:
        dotFile.append("node[fixedsize=true,width=0.2,height=0.2]")
    #dotFile.append("node[shape=box]")
    segmentGroups = g.graph['gProp']
    i = 0
    for n in g.nodes():
        i += 1
        nodeLabel = "[label=\"\"".format(i)
        if 'proximal' in g.node[n].keys():
            x = (float(g.node[n]['proximal'].get('x')) + float(g.node[n]['distal'].get('x')))/2
            y = (float(g.node[n]['proximal'].get('y')) + float(g.node[n]['distal'].get('y')))/2
            z = (float(g.node[n]['proximal'].get('z')) + float(g.node[n]['distal'].get('z')))/2
            nodeLabel += "pos=\"{0},{1},{2}!\"".format(x,y,z)

        # color the segment groups
        if g.node[n]['color']:
            nodeLabel += ",style=filled,color=\""+g.node[n]['color']+"\""
        segG = g.node[n].get('segmentGroup')
        if segG:
            colorName = segGrp2Color(segG, g.graph['gProp']['segmentGroup'])
            nodeLabel += "style=filled,"+colorName+","

        nodeLabel += "]"
        dotFile.append(g.node[n]['label'] + " " + nodeLabel)
    for (f, t) in g.edges():
        edge = g.node[f]['label'] + " -> " + g.node[t]['label']
        edge += " ["
        if 'synapse' in g.edge[f][t].keys():
            # color this edge and make it look like a synapse
            edge += " label=\"{0}\",".format(g.edge[f][t]['id'])
            edge += "color=blue,style=dotted,"
        edge += "]"
        dotFile.append(edge)

    # End the file
    dotFile.append("}")
    txt = "\n".join(dotFile)
    with open(filename, "w") as outFile:
        outFile.write(txt)
