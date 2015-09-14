from tkinter import *
import os
from PIL import Image, ImageTk
from subprocess import call

'''Pillow - for Tkinter .bmp support: https://pypi.python.org/pypi/Pillow/2.9.0
   If using a version of python before 3.x, I think the PIL library should work instead of Pillow. '''

root = Tk()
    
def updateCfgFile(sceneCopy, maxRayDepthCopy, superSampCopy, topTreeDepthCopy, triObjTreeDepthCopy, outFileNameCopy):   
    print('Updating Cfg File...')
    print('Scene: ' + sceneCopy)
    print('Max Ray Depth: ' + str(maxRayDepthCopy))
    print('Super-sample Factor: ' + str(superSampCopy))
    print('Top Lvl. Tree Depth: ' + str(topTreeDepthCopy))
    print('TriObj Tree Depth: ' + str(triObjTreeDepthCopy))
    print('Output Image: ' + outFileNameCopy)
    
    cfgFile = open('../config.txt', 'w')
    rootPath = os.getcwd() + '/../'
    cfgFile.write('SCENE_PATH ' + rootPath + 'Scenes/' + sceneCopy + '\n')
    cfgFile.write('OUTPUT_IMG ' + rootPath + 'Output/' + outFileNameCopy + '\n')
    cfgFile.write('MAX_RAY_DEPTH ' + str(maxRayDepthCopy) + '\n')
    cfgFile.write('PARAM_LIN_SUPERSAMPLE_FACTOR ' + str(superSampCopy) + '\n')
    cfgFile.write('TOP_LEVEL_OBJECT_TREE_DEPTH ' + str(topTreeDepthCopy) + '\n')
    cfgFile.write('TRI_TREE_DEPTH ' + str(triObjTreeDepthCopy) + '\n')
    
    cfgFile.close()
    
def startRender():    
    global root
    global scene
    global maxRayDepth
    global superSamp
    global topTreeDepth
    global triObjTreeDepth
    global outFileName
    global imFrame
    global imCanvas
    global rowNum
    global ph

    # Local snapshot of changeable values, taken so that user doesn't modify values in between accesses
    sceneCopy = str(scene.get())
    maxRayDepthCopy = maxRayDepth.get()
    superSampCopy = superSamp.get()
    topTreeDepthCopy = topTreeDepth.get()
    triObjTreeDepthCopy = triObjTreeDepth.get()
    outFileNameCopy = str(outFileName.get())
    
    dispImgName = 'defaultBgImg.bmp' 
    im = Image.open(dispImgName)
    ph = ImageTk.PhotoImage(im)
    imCanvas.delete(ALL)
    imCanvas.create_image(0,0,image=ph, anchor="nw")
    imFrame.grid(row=0, column=1, rowspan=rowNum, columnspan=1)
    im.close()
    
    # Force Tkinder to update its processing, otherwise default img doesn't get drawn until we exit this callback processing
    # and resume mainloop.
    #root.update_idletasks()
    root.update()
    
    updateCfgFile(sceneCopy, maxRayDepthCopy, superSampCopy, topTreeDepthCopy, triObjTreeDepthCopy, outFileNameCopy)
    call('rayTrace') # Call the rayTracer. Assumes Path variable set up so system can find it
    
    dispImgName = '../Output/' + outFileNameCopy
    im = Image.open(dispImgName)
    ph = ImageTk.PhotoImage(im)
    imCanvas.delete(ALL)
    imCanvas.create_image(0,0,image=ph, anchor="nw")
    imFrame.grid(row=0, column=1, rowspan=rowNum, columnspan=1)
    im.close()
    
def runGui():
    global root
    global scene
    global maxRayDepth
    global superSamp
    global topTreeDepth
    global triObjTreeDepth
    global outFileName
    global imFrame
    global imCanvas
    global rowNum
    global ph #global so that image doesn't go out of scope at callback exit
    
    rowNum = 0
    
    root.title('rayTrace GUI')
    
    sceneList = os.listdir('../Scenes')
    Label(root, text='Scene Select').grid(row=rowNum, sticky=W)
    rowNum = rowNum + 1
    scene = StringVar(root)
    scene.set(sceneList[0]) # default value, can call scene.get() later to get updated value
    menuArg = (root, scene)  + tuple(sceneList)
    w = OptionMenu(*menuArg)
    w.grid(row=rowNum, sticky=W) 
    rowNum = rowNum + 1
    
    Label(root, text='Max Ray Depth').grid(row=rowNum, sticky=W)
    rowNum = rowNum + 1
    maxRayDepth = IntVar(root)
    maxRayDepth.set(2) # default to depth 2
    menuArg = (root, maxRayDepth) + tuple(range(0,16)) #don't allow too deep recursion via GUI
    w = OptionMenu(*menuArg)
    w.grid(row=rowNum, sticky=W)
    rowNum = rowNum + 1

    Label(root, text='Linear Super-sample Factor').grid(row=rowNum, sticky=W)
    rowNum = rowNum + 1
    superSamp = IntVar(root)
    superSamp.set(1) #default
    menuArg = (root, superSamp) + tuple(range(1,11)) #don't allow too deep recursion via GUI
    w = OptionMenu(*menuArg)
    w.grid(row=rowNum, sticky=W)
    rowNum = rowNum + 1

    Label(root, text='Top Lvl. Tree Depth').grid(row=rowNum, sticky=W)
    rowNum = rowNum + 1
    topTreeDepth = IntVar(root)
    topTreeDepth.set(3) # default
    menuArg = (root, topTreeDepth) + tuple(range(1,31)) #don't allow too deep recursion via GUI
    w = OptionMenu(*menuArg)
    w.grid(row=rowNum, sticky=W)
    rowNum = rowNum + 1

    Label(root, text='TriObj Tree Depth').grid(row=rowNum, sticky=W)
    rowNum = rowNum + 1
    triObjTreeDepth = IntVar(root)
    triObjTreeDepth.set(5) # default
    menuArg = (root, triObjTreeDepth) + tuple(range(1,31)) #don't allow too deep recursion via GUI
    w = OptionMenu(*menuArg)
    w.grid(row=rowNum, sticky=W)    
    rowNum = rowNum + 1
   
    Label(root, text='Output Image').grid(row=rowNum, sticky=W)
    rowNum = rowNum + 1
    outFileName = Entry(root)
    outFileName.delete(0, END)
    outFileName.insert(0, 'rayTraceOutput.bmp')
    outFileName.grid(row=rowNum, sticky=W)
    rowNum = rowNum + 1
   
    b = Button(root, text='Render!', command=startRender)
    b.grid(row=rowNum)
    rowNum = rowNum + 1
   
    # Display the default img if no output of the same name as the new output is available (assumes the same
    # name corresponds to a previous run, which won't really hurt anything if wrong).
    dispImgName = 'defaultBgImg.bmp'
    if str(outFileName.get()) in os.listdir('../Output'):
        dispImgName = '../Output/' + str(outFileName.get()) #TODO: converting from Entry to string, might be unneeded, investigate further 
  
    ###
    ''' scrollable canvas based on http://stackoverflow.com/questions/5382039/scrollbars-for-a-jpg-image-on-a-tkinter-canvas-in-python '''
    imFrame = Frame(root)
    imFrame.grid_rowconfigure(0, weight=1)
    imFrame.grid_columnconfigure(0, weight=1)

    xscrollbar = Scrollbar(imFrame, orient=HORIZONTAL)
    xscrollbar.grid(row=1, column=0, sticky=E+W)

    yscrollbar = Scrollbar(imFrame)
    yscrollbar.grid(row=0, column=1, sticky=N+S)

    imCanvas = Canvas(imFrame, bd=0, xscrollcommand=xscrollbar.set, yscrollcommand=yscrollbar.set)
    imCanvas.grid(row=0, column=0, sticky=N+S+E+W)

    im = Image.open(dispImgName)
    ph = ImageTk.PhotoImage(im)
    imCanvas.create_image(0,0,image=ph, anchor="nw")
    imCanvas.config(scrollregion=imCanvas.bbox(ALL))
    
    xscrollbar.config(command=imCanvas.xview)
    yscrollbar.config(command=imCanvas.yview)
    
    imFrame.grid(row=0, column=1, rowspan=rowNum, columnspan=1)
    ###
        
    root.mainloop()

if __name__ == '__main__':
    runGui()