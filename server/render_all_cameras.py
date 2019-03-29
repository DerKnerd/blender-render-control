import bpy
import os
import sys

context = bpy.context
user_preferences = context.user_preferences
scene = context.scene
objects = scene.objects

argv = sys.argv[sys.argv.index("--") + 1:]
fullpath = sys.argv[1]
filename = os.path.splitext(os.path.basename(fullpath))[0] + " - "
foldername = os.path.dirname(fullpath)

scene.render.resolution_x = int(argv[0])
scene.render.resolution_y = int(argv[1])
scene.render.resolution_percentage = 100
scene.render.engine = "CYCLES"
scene.render.use_border = False

scene.cycles.device = "CPU"
scene.cycles.tile_order = "CENTER"
scene.cycles.tile_x = 512
scene.cycles.tile_y = 512

preferences = context.user_preferences
cycles_preferences = preferences.addons['cycles'].preferences

# Attempt to set GPU device types if available
for compute_device_type in ('CUDA', 'OPENCL', 'NONE'):
    try:
        cycles_preferences.compute_device_type = compute_device_type
        break
    except TypeError:
        pass

# Enable all CPU and GPU devices
for device in cycles_preferences.devices:
    device.use = True

count = 0
output_dir = foldername + '/'

print('Output directory: ' + output_dir)
for obj in objects:
    if obj.type.upper() == 'CAMERA':
        print(obj.name)
        count += 1
        scene.camera = obj
        bpy.ops.render.render()

        print('Rendering with camera: ' + obj.name)
        render = bpy.data.images['Render Result'].copy()

        file_name = filename + obj.name + scene.render.file_extension
        print('File created: "' + file_name + '"')
        render.save_render(output_dir + file_name)

print(str(count) + " files created at: " + output_dir)
