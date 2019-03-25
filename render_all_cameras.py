import os
import sys
import bpy

context = bpy.context
user_preferences = context.user_preferences
scene = context.scene
objects = scene.objects

for obj in objects:
    obj.select = obj.type == "CAMERA"

file_prefix = os.path.splitext(bpy.path.basename(context.blend_data.filepath))[0] + " - "

argv = sys.argv[sys.argv.index("--") + 1:]
scene.render.resolution_x = int(argv[0])
scene.render.resolution_y = int(argv[1])
scene.render.resolution_percentage = 100
scene.render.engine = "CYCLES"
scene.cycles.device = "GPU"
scene.render.use_border = False
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
output_dir = bpy.path.abspath('//')

print('Output directory: ' + output_dir)
for obj in context.selected_objects:
    if obj.type == 'CAMERA':
        count += 1
        scene.camera = obj
        bpy.ops.render.render()

        print('Rendering with camera: ' + obj.name)
        render = bpy.data.images['Render Result'].copy()

        file_name = file_prefix + obj.name + scene.render.file_extension
        print('File created: "' + file_name + '"')
        render.save_render(output_dir + file_name)

print(str(count) + " files created at: " + output_dir)
