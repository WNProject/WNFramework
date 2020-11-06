A sample obj file exists here http://www.crytek.com/cryengine/cryengine3/downloads
If you are using this one, apply the patch mtl.diff to fix some errors in the file.

Make sure you have astcenc on your path. You can get it from the Mali SDK.
https://developer.arm.com/products/software-development-tools/graphics-development-tools/mali-texture-compression-tool

If you want to use BC encoding, then get Nvidia texture tools, and add it to your path
https://developer.nvidia.com/gpu-accelerated-texture-compression

Apply the diff from this directory to the mtl file.

Run
```
cd <build_directory>

python compile_obj.py --objfile path/to/obj --output-file scene.scn

python compile_shaders.py --source path/to/demo/vct/shaders
```
This will generate output in `<build_directory>` directory.

`compile_obj.py --bc` will generate textures with BC1 compression otherwise ASTC will be generated. Is is actually suggested to use --bc since it has wider support (but is larger).

`compile_shaders.py --d3d12` will only generate d3d12 shaders. DXC must be on your path.

`compile_shaders.py --vulkan` will only generate Vulkan shaders. DXC with Vulkan support must be on your path
A version of DXC can be gotten from [here](http://www.khr.io/dxcappveyorbuild)

`compile_shaders.py` will generate both Vulkan and D3D12 shaders. DXC with vulkan support must be on your path.

When running the executable, do it from the directory that contains the output.
Or pass it in as a command-line argument:
```
./vct --data_dir <build_directory>
```