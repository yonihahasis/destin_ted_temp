import os, errno
import cv2.cv as cv
import time
import pydestin as pd


experiment_root_dir="./experiment_runs"


ims = pd.ImageSouceImpl()

letters = "ABCDEFGHIJKLMNOP"
for l in letters:
    ims.addImage("/home/ted/Pictures/%s.png" % l)
    

centroids =  [2,4,8,32,64,64,64,16]   
layers = len(centroids)
top_layer = layers - 1
dn = pd.DestinNetworkAlt( pd.W512, 8, centroids, True)
dn.setFixedLearnRate(.1)
dn.setBeliefTransform(pd.DST_BT_BOLTZ)
dn.setTemperatures([1.2,10,5,5,5,5,5,5])

weight_exponent = 4

save_root="./saves/"

def train():
    for i in range(6400):
        if i % 10 == 0:
            print "Iteration " + str(i)
            
        ims.findNextImage()
        #dn.clearBeliefs()
        for j in range(1):
            
            f = ims.getGrayImageFloat()    
            dn.doDestin(f)

#display centroid image
def dci(layer, cent, equalize_hist = False, exp_weight = 4):
    if cent >= dn.getBeliefsPerNode(layer):
        print "centroid out bounds"
        return
    dn.setCentImgWeightExponent(exp_weight)
    dn.displayCentroidImage(layer, cent, 256, equalize_hist)
    cv.WaitKey(100)

def dcis(layer):
    dn.displayLayerCentroidImages(layer,1000)
    cv.WaitKey(100)
    

def mkdir(path):
    try:
        os.makedirs(path)
    except OSError as exc: # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else: raise


def window_callback(event, x, y, flag, param):
    if event == cv.CV_EVENT_LBUTTONUP:
        pass
    
def saveCenImages(run_id):
    run_dir = experiment_root_dir + "/"+run_id+"/"
    mkdir(run_dir)
    
    # with centroid weight = 1 
    orig_dir = run_dir+"orig/"
    
    # with centroid weight = 1 and enhanced = true
    orige_dir = run_dir+"orig_e/"
    
    # store centroid with higher weighting
    highweighted_dir = run_dir+"highweight/"
    
    highweightede_dir = run_dir+"highweight_e/"

    for d in [orig_dir, orige_dir, highweighted_dir, highweightede_dir]:
        mkdir(d)
    
    # Save centriod images
    bn = dn.getBeliefsPerNode(top_layer)
        
    dn.setCentImgWeightExponent(1)
    dn.updateCentroidImages()
    for i in range(bn):
        f = "%02d_%s.png" % (i,run_id)
        # save original
        fn = orig_dir + f 
        dn.saveCentroidImage(top_layer, i, fn, 512, False )
    
        # save original enhanced
        fn = orige_dir + f
        dn.saveCentroidImage(top_layer, i, fn, 512, True )
    
    
    dn.setCentImgWeightExponent(weight_exponent)
    dn.updateCentroidImages()
    for i in range(bn):
        f = "%02d_%s.png" % (i,run_id)
        fn = highweighted_dir + f 
        dn.saveCentroidImage(top_layer, i, fn, 512, False )
        fn = highweightede_dir + f
        dn.saveCentroidImage(top_layer, i, fn, 512, True )    
    
#dn.load("x.dst")
#cv.SetMouseCallback("Centroid image",window_callback)
do_train = True
save = False
if do_train:
    train()
    if save:    
        t = str(int(time.time()))
        fn = save_root + t + ".dst"
        print "Saving " + fn
        dn.save(fn)
        saveCenImages(t)
else:
    to_load = "saves/1358139144.dst"
    dn.load(to_load)
    
dci(7,0,False, weight_exponent)
dcis(7)
    
    