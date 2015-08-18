from sys import *

''' sphereArray <rad> <nX> <nY> <nZ> ''' 
def sphereArray(rad, nX, nY, nZ):
    curX = -rad*(nX-1)
    curY = -rad*(nY-1)
    curZ = -rad*(nZ-1)
    for sx in range(0,nX):
        curY = -rad*(nY-1)
        for sy in range(0,nY):
            curZ = -rad*(nZ-1)
            for sz in range(0,nZ):
                print("SP " + str(curX) + " " + str(curY) + " " + str(curZ) + " " + str(rad) +
                      " 255 255 255 1.2 1.0 0.0 1.0 1.0 0.01")
                curZ += 2*rad
            curY += 2*rad				
        curX += 2*rad			
    
if __name__ == '__main__':
    script, rad, nX, nY, nZ = argv
    sphereArray(float(rad), int(nX), int(nY), int(nZ))