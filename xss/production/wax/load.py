'''
Created on 18/09/2012

@author: kalandraka
'''
import bpy
import sys

def import_model(path): 
    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.object.delete()
    if ('.blend' in path):
        bpy.ops.wm.open_mainfile(filepath=path)
    elif ('.dae' in path):
        bpy.ops.wm.collada_import(filepath=path)
    elif ('.3ds' in path):
        bpy.ops.import_scene.autodesk_3ds(filepath=path)
    elif ('.obj' in path):
        bpy.ops.import_scene.obj(filepath=path)       

model_path = sys.argv[5]

import_model(model_path)

bpy.ops.wm.collada_export(filepath=model_path+'.dae')
