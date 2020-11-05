from PIL import Image
import array
import math
import os
import shutil
import subprocess
import tempfile


# TODO: make this configurable somehow
ASTC_BLOCK_SIZE_X = 8
ASTC_BLOCK_SIZE_Y = 8

ASTC_BYTES_PER_BLOCK = 128/8

ASTC_HEADER_SIZE = 16
# https://stackoverflow.com/questions/22600678/determine-internal-format-of-given-astc-compressed-image-through-its-header
# struct astc_header
# {
#    uint8_t magic [ 4 ];
#    uint8_t blockdim_x;
#    uint8_t blockdim_y;
#    uint8_t blockdim_z ;
#    uint8_t xsize [ 3 ];
#    uint8_t ysize [ 3 ];
#    uint8_t zsize [ 3 ];
#};

IMAGE_FORMAT_ASTC = 1
IMAGE_FORMAT_BC1 = 2
IMAGE_FORMAT_BC1N = 3

BC_BLOCK_SIZE_Y = 4
BC_BLOCK_SIZE_X = 4
BC1_BYTES_PER_BLOCK = 8
DDS_HEADER_SIZE = 128

# Compressed texture format
# uint32 ImageFormat
# uint32 ImageWidth
# uint32 ImageHeight
# uint32 BlockSizeX
# uint32 BlockSizeY
# uint32 NumMips
# {uint32 MipOffset, uint32 MipSize} * N
# ALL THE MIP DATA

IMAGE_HEADER_SIZE = 24


class mipLevel(object):
    def __init__(self):
        self.offset = 0
        self.data = None


def convert_astc_texture(filename, output_name, fast, is_normal, generate_mips):
    if not os.path.isfile(filename):
        print "Could not find " + filename
        return ""
    image_dir = tempfile.mkdtemp()
    tempname = os.path.basename(output_name)
    try:
        mip_levels = []
        image = Image.open(filename).convert("RGB")
        width = image.size[0]
        height = image.size[1]
        total_data = 0
        idx = 0
        while True:
            out_file = os.path.join(image_dir, tempname + str(idx) + ".tga")
            ast_file = os.path.join(image_dir, tempname + str(idx) + ".astc")
            o = image.resize((width, height), Image.BICUBIC)

            o.save(out_file)
            args = ['astcenc', '-c', out_file,
                    ast_file, '{}x{}'.format(ASTC_BLOCK_SIZE_X, ASTC_BLOCK_SIZE_Y)]
            if fast:
                args.append('-fast')
            else:
                args.append('-thorough')
            subprocess.call(args)

            block_width = int(math.ceil(float(width) / ASTC_BLOCK_SIZE_X))
            block_height = int(
                math.ceil(float(height) / ASTC_BLOCK_SIZE_Y))

            astc_data_size = int(
                block_width * block_height * ASTC_BYTES_PER_BLOCK)
            if os.path.getsize(ast_file) != ASTC_HEADER_SIZE + astc_data_size:
                print "File was the wrong size. Got {}, expected {}".format(
                    os.path.getsize(ast_file), ASTC_HEADER_SIZE + astc_data_size)
            with open(ast_file, "rb") as output_file:
                # Read the header, we throw it out anyway
                array.array("B").fromfile(output_file, ASTC_HEADER_SIZE)
                b = array.array("B")
                b.fromfile(output_file, astc_data_size)
                mip = mipLevel()
                mip.data = b
                mip.offset = total_data
                mip_levels.append(mip)

            total_data += astc_data_size
            if (width == 1 and height == 1) or not generate_mips:
                break
            width = int(math.ceil(float(width) / 2.0))
            height = int(math.ceil(float(height) / 2.0))
            idx += 1

        with open(output_name, "wb") as f:
            array.array(
                "L", [IMAGE_FORMAT_ASTC, image.size[0], image.size[1], ASTC_BLOCK_SIZE_X, ASTC_BLOCK_SIZE_Y, len(mip_levels)]).tofile(f)
            for m in mip_levels:
                m.offset += IMAGE_HEADER_SIZE + len(mip_levels) * 8
            for m in mip_levels:
                array.array("L", [m.offset, len(m.data)]).tofile(f)
            for m in mip_levels:
                m.data.tofile(f)
    except Exception as e:
        print e
        shutil.rmtree(image_dir)
        return ""
    shutil.rmtree(image_dir)
    return output_name


def convert_bc_texture(filename, output_name, fast, is_normal, generate_mips):
    if not os.path.isfile(filename):
        print "Could not find " + filename
        return ""

    image_dir = tempfile.mkdtemp()
    tempname = os.path.basename(output_name)

    try:
        mip_levels = []
        image = Image.open(filename).convert("RGB")
        image = image.transpose(Image.FLIP_TOP_BOTTOM)
        width = image.size[0]
        height = image.size[1]
        total_data = 0
        idx = 0
        while True:
            out_file = os.path.join(image_dir, tempname + str(idx) + ".tga")
            dds_file = os.path.join(image_dir, tempname + str(idx) + ".dds")
            o = image.resize((width, height), Image.BICUBIC)

            o.save(out_file)
            args = ['nvcompress', '-nomips', '-nocuda']
            if is_normal:
                args.append('-normal')
                args.append('-bc1n')
            else:
                args.append('-color')
                args.append('-bc1')

            if fast:
                args.append('-fast')

            args.append(out_file)
            args.append(dds_file)
            subprocess.call(args)

            block_width = int(math.ceil(float(width) / BC_BLOCK_SIZE_X))
            block_height = int(math.ceil(float(height) / BC_BLOCK_SIZE_Y))

            data_size = int(block_width * block_height *
                            BC1_BYTES_PER_BLOCK)
            if os.path.getsize(dds_file) != DDS_HEADER_SIZE + data_size:
                print "File was the wrong size. Got {}, expected {}".format(
                    os.path.getsize(dds_file), DDS_HEADER_SIZE + data_size)
            with open(dds_file, "rb") as output_file:
                # Throw away the header data
                array.array("B").fromfile(output_file, DDS_HEADER_SIZE)
                b = array.array("B")
                b.fromfile(output_file, data_size)
                mip = mipLevel()
                mip.data = b
                mip.offset = total_data
                mip_levels.append(mip)

            total_data += data_size
            if (width == 1 and height == 1) or not generate_mips:
                break
            width = int(math.ceil(float(width) / 2.0))
            height = int(math.ceil(float(height) / 2.0))
            idx += 1

        with open(output_name, "wb") as f:
            format = IMAGE_FORMAT_BC1
            if is_normal:
                format = IMAGE_FORMAT_BC1N
            array.array(
                "L", [format, image.size[0], image.size[1], BC_BLOCK_SIZE_X, BC_BLOCK_SIZE_Y, len(mip_levels)]).tofile(f)
            for m in mip_levels:
                m.offset += IMAGE_HEADER_SIZE + len(mip_levels) * 8
            for m in mip_levels:
                array.array("L", [m.offset, len(m.data)]).tofile(f)
            for m in mip_levels:
                m.data.tofile(f)
    except Exception as e:
        print e
        shutil.rmtree(image_dir)
        return ""
    # shutil.rmtree(image_dir)
    return output_name
