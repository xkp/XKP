'''
Created on 18/09/2012

@author: kalandraka
'''
import bpy
import sys
from xml.dom import minidom

# baking function

def bake_image(bake_type, output):        
    
    scene = bpy.data.scenes[0]
    
    for m in bpy.data.meshes:
        
        key = m.name
    
        # select object
        bpy.ops.object.select_all(action='DESELECT')
        bpy.ops.object.select_pattern(pattern=key)
    
        bpy.ops.mesh.uv_texture_add()
        bpy.ops.object.editmode_toggle()
        bpy.ops.uv.smart_project(angle_limit=66)
    
        bpy.ops.image.new(name=key, width=1024, height=1024, float=True)
        bpy.data.screens['UV Editing'].areas[1].spaces[0].image = bpy.data.images[key]
    
        bpy.ops.object.editmode_toggle()
    
        scene.render.bake_type = bake_type
        scene.render.use_bake_selected_to_active = True
        bpy.ops.object.bake_image()
        bpy.data.images[key].save_render(output + key + ".png")
    
        bpy.ops.mesh.uv_texture_remove()
		
# translate function

def translate(x, y, z):
	bpy.ops.object.select_all(action='DESELECT')
	bpy.ops.object.select_by_type()
	bpy.ops.transform.translate(value=(x, y, z))

# rotate function

def rotate(val, x, y, z):
    bpy.ops.object.select_all(action='DESELECT')
    bpy.ops.object.select_by_type()
    bpy.ops.transform.rotate(value=val, axis=(x, y, z))

# scale function

def scale(x, y, z):
    bpy.ops.object.select_all(action='DESELECT')
    bpy.ops.object.select_by_type()
    bpy.ops.transform.resize(value=(x, y, z))

# input/output functions

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
        
def export_model(path):    
    if ('.blend' in path):        
        bpy.ops.wm.save_as_mainfile(filepath=path)
    elif ('.dae' in path):
        bpy.ops.wm.collada_export(filepath=path)
    elif ('.3ds' in path):
        bpy.ops.export_scene.autodesk_3ds(filepath=path)
    elif ('.obj' in path):
        bpy.ops.export_scene.obj(filepath=path)
        
# parser

model_path = sys.argv[5]
xml_path = sys.argv[6]
xmldoc = minidom.parse("test.xml")

bake_list = xmldoc.getElementsByTagName("bake")
scale_list = xmldoc.getElementsByTagName("scale")
rotate_list = xmldoc.getElementsByTagName("rotate")
translate_list = xmldoc.getElementsByTagName("translate")
export_list = xmldoc.getElementsByTagName("export")

# import loaded model
import_model(model_path)

for b in bake_list :
    bake_image(b.attributes['bake_type'].value, b.attributes['output'].value)

for s in scale_list :
    scale(float(s.attributes['x'].value), float(s.attributes['y'].value), float(s.attributes['z'].value))

for r in rotate_list :
    rotate(float(r.attributes['x'].value), float(r.attributes['y'].value), float(r.attributes['z'].value))
	
for t in translate_list :
    translate(float(t.attributes['x'].value), float(t.attributes['y'].value), float(t.attributes['z'].value))

for e in export_list :
    export_model(e.attributes['path'].value)
