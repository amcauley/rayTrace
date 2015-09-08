from sys import *

''' obj2TriObj <objFile> <outFile> ''' 
def obj2TriObj(objFile, outFile):
    fin = open(objFile, 'r')
    fout = open(outFile, 'w')
    
    verts = []
    for line in fin:
        if (line[0] == 'v'):  #vertex definition, ex. "v -0.136296 0.0938588 -0.0307373"
            vals = line.split()
            verts.append((vals[1], vals[2], vals[3]))
        elif (line[0] == 'f'): #geometry/triangle definition referencing vertex num (starting from 1), ex. "f 1 2 3"
            vals = line.split()
            vert1, vert2, vert3 = int(vals[1])-1, int(vals[2])-1, int(vals[3])-1
            fout.write(verts[vert1][0] + " " + verts[vert1][1] + " " + verts[vert1][2] + " " + \
                       verts[vert2][0] + " " + verts[vert2][1] + " " + verts[vert2][2] + " " + \
                       verts[vert3][0] + " " + verts[vert3][1] + " " + verts[vert3][2] + "\n")
    
    fin.close()
    fout.close()
    
if __name__ == '__main__':
    script, objFile, outFile = argv
    obj2TriObj(objFile, outFile)