import os
import re
import textures


class material(object):
    def __init__(self):
        self.diffuse = ""
        self.spec = ""
        self.norm = ""
        self.mask = ""


def parse_material_file(mat, bc, encode, fast_encode):
    mtl_header_re = re.compile(r'newmtl\s+(.*)')
    diff_map_re = re.compile(r'\s*map_Kd\s+(.*)')
    mask_re = re.compile(r'\s*map_d\s+(.*)')
    norm_map_re = re.compile(r'\s*map_bump\s+(.*)')
    spec_re = re.compile(r'\s*map_Ks\s+(.*)')

    this_material = None
    all_materials = {}
    all_textures = {}

    convert_texture_func = textures.convert_astc_texture
    if bc:
        convert_texture_func = textures.convert_bc_texture

    def convert_texture(filename, normal):
        if filename in all_textures:
            return all_textures[filename]
        name = os.path.basename(filename)
        name += ".img"
        if not encode:
            all_textures[filename] = name
            return name
        ret = convert_texture_func(filename, name, fast_encode, normal, True)
        all_textures[filename] = ret
        return ret

    with open(mat, "r") as f:
        material_dir = os.path.dirname(mat)
        for l in f.readlines():
            header_match = mtl_header_re.match(l)
            diff_match = diff_map_re.match(l)
            mask_match = mask_re.match(l)
            norm_match = norm_map_re.match(l)
            spec_match = spec_re.match(l)
            if header_match:
                if this_material:
                    all_materials[this_material.name] = this_material
                this_material = material()
                this_material.name = header_match.group(1)
            elif diff_match:
                this_material.diffuse = convert_texture(
                    os.path.join(material_dir, diff_match.group(1)), False)
            elif spec_match:
                this_material.spec = convert_texture(
                    os.path.join(material_dir, spec_match.group(1)), False)
            elif mask_match:
                this_material.mask = convert_texture(
                    os.path.join(material_dir, mask_match.group(1)), False)
            elif norm_match:
                this_material.norm = convert_texture(
                    os.path.join(material_dir, norm_match.group(1)), True)
    if this_material:
        all_materials[this_material.name] = this_material
    return all_materials
