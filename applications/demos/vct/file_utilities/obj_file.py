import array
import materials
import os
import re
import math


class element_vertex(object):
    def __init__(self):
        self.x = 0.0
        self.y = 0.0
        self.z = 0.0
        self.nx = 0.0
        self.ny = 0.0
        self.nz = 0.0
        self.u = 0.0
        self.v = 0.0
        self.tanx = 0.0
        self.tany = 0.0
        self.tanz = 0.0
        self.tanw = 0.0
        self.tan1x = 0.0
        self.tan1y = 0.0
        self.tan1z = 0.0
        self.tan2x = 0.0
        self.tan2y = 0.0
        self.tan2z = 0.0

    def __str__(self):
        return "<{},{},{},{},{},{},{},{}>".format(
            self.x, self.y, self.z,
            self.nx, self.ny, self.nz,
            self.u, self.v)


class element(object):
    def __init__(self):
        self.indices = []
        self.vertices = []
        self.name = ""
        self.pos = position()
        self.name_offset = 0
        self.material_offset = 0
        self.vert_offset = 0
        self.index_offset = 0
        self.aabb = aabb()
        self.diffuse = ""
        self.spec = ""
        self.mask = ""
        self.norm = ""
        self.diffuse_offset = ""
        self.spec_offset = ""
        self.mask_offset = ""
        self.norm_offset = ""

    def __str__(self):
        return "{}: vert:{} ind:{}, pos:{}, aabb:{}".format(
            self.name,
            len(self.vertices),
            len(self.indices),
            self.pos,
            self.aabb
        )

    def reset_origin(self):
        x = self.vertices[0].x
        y = self.vertices[0].y
        z = self.vertices[0].z
        maxx = x
        maxy = y
        maxz = z
        for v in self.vertices:
            if v.x < x:
                x = v.x
            else:
                maxx = v.x

            if v.y < y:
                y = v.y
            else:
                maxy = v.y

            if v.z < z:
                z = v.z
            else:
                maxz = v.z
        for v in self.vertices:
            v.x -= x
            v.y -= y
            v.z -= z
        self.pos.x = x
        self.pos.y = y
        self.pos.z = z

        self.aabb.minx = x
        self.aabb.miny = y
        self.aabb.minz = z
        self.aabb.maxx = maxx
        self.aabb.maxy = maxy
        self.aabb.maxz = maxz

        for v in self.vertices:
            ndt = v.tan1x * v.nx + v.tan1y * v.ny + v.tan1z * v.nz
            tmnxndtx = v.tan1x - (ndt * v.nx)
            tmnxndty = v.tan1y - (ndt * v.ny)
            tmnxndtz = v.tan1z - (ndt * v.nz)

            tot = math.sqrt(tmnxndtx*tmnxndtx + tmnxndty *
                            tmnxndty + tmnxndtz*tmnxndtz)

            if (tot == 0):
                tot = 1

            v.tanx = tmnxndtx / tot
            v.tany = tmnxndty / tot
            v.tanz = tmnxndtz / tot

            # Cross(n, t)
            crssx = v.ny * v.tan1z - v.nz * v.tan1y
            crssy = v.nz * v.tan1x - v.nx * v.tan1z
            crssz = v.nx * v.tan1y - v.ny * v.tan1x

            # Dot(crssx, t2)
            dp = crssx * v.tan2x + crssy * v.tan2y + crssz * v.tan2z
            if dp < 0:
                dp = -1
            else:
                dp = 1
            v.tanw = dp


class position(object):
    def __init__(self):
        self.x = 0.0
        self.y = 0.0
        self.z = 0.0

    def __str__(self):
        return "<{},{},{}>".format(self.x, self.y, self.z)


class aabb(object):
    def __init__(self):
        self.minx = 0.0
        self.miny = 0.0
        self.minz = 0.0
        self.maxx = 0.0
        self.maxy = 0.0
        self.maxz = 0.0

    def __str__(self):
        return "[<{},{},{}>, <{},{},{}>]".format(
            self.minx,
            self.miny,
            self.minz,
            self.maxx,
            self.maxy,
            self.maxz,
        )


class texcoord(object):
    def __init__(self):
        self.u = 0.0
        self.y = 0.0

    def __str__(self):
        return "{{},{}}".format(self.u, self.v)


class normal(object):
    def __init__(self):
        self.nx = 0.0
        self.ny = 0.0
        self.nz = 0.0

    def __str__(self):
        return "[{},{},{}]".format(self.nx, self.ny, self.nz)


OBJECT_HEADER_LENGTH = 18*4
'''
The header looks like <all little endian>:
u32 NameOffset
u32 NameLength
u32 VertOffset
u32 VertCount
u32 IndexOffset
u32 IndexCount
u32 Diff_tex_offset
u32 Diff_tex_len
u32 Spec_tex_offset
u32 Spec_tex_len
u32 Mask_tex_offset
u32 Mask_tex_len
u32 norm_tex_offset
u32 norm_tex_len
float, float, float position
u32 reserved
'''

VERTEX_LENGTH = 12 * 4
'''
A vertex looks like <all little endian>:
float x, y, z
float nx, ny, nz
float u, float v
'''


INDEX_SIZE = 4
'''
For now: All indices are 32-bit
'''

BC_ENCODING = 0
ASTC_ENCODING = 1


def compile_obj_file(filename, output_file, texture_encoding, ignore_textures, fast_encode):
    positions = []
    texcoords = []
    normals = []

    total_name_size = 0
    total_vertex_size = 0
    total_index_size = 0

    current_vertices = {}
    this_element = None
    all_elements = []

    pos_re = re.compile(r'v\s+(-?\d+\.\d+)\s+(-?\d+\.\d+)\s+(-?\d+\.\d+)')
    tc_re = re.compile(r'vt\s+(-?\d+\.\d+)\s+(-?\d+\.\d+)\s+')
    norm_re = re.compile(r'vn\s+(-?\d+\.\d+)\s+(-?\d+\.\d+)\s+(-?\d+\.\d+)')
    vertex_re = re.compile(r'(\d+)/(\d+)/(\d+)')
    tri_face_re = re.compile(
        r'f\s+(\d+/\d+/\d+)\s+(\d+/\d+/\d+)\s+(\d+/\d+/\d+)')
    quad_face_re = re.compile(
        r'f\s+(\d+/\d+/\d+)\s+(\d+/\d+/\d+)\s+(\d+/\d+/\d+)\s+(\d+/\d+/\d+)')
    obj_re = re.compile(r'g\s+([^\s]+)')
    mtl_re = re.compile(r'mtllib\s+(.*)')
    usemtl_re = re.compile(r'usemtl\s+(.*)')

    all_materials = {}

    bc = (texture_encoding == BC_ENCODING)

    def get_vertex(vert_matches):
        pos = positions[int(vert_matches.group(1)) - 1]
        tc = texcoords[int(vert_matches.group(2)) - 1]
        norm = normals[int(vert_matches.group(3)) - 1]
        v = element_vertex()
        v.x = pos.x
        v.y = pos.y
        v.z = pos.z
        v.nx = norm.nx
        v.ny = norm.ny
        v.nz = norm.nz
        v.u = tc.u
        v.v = tc.v
        if str(v) in current_vertices:
            return current_vertices[str(v)]
        current_vertices[str(v)] = len(current_vertices)
        this_element.vertices.append(v)
        return len(current_vertices) - 1

    # Calculates the tangent vector for the vertex.
    def process_triangle(i1, i2, i3):
        v1 = this_element.vertices[i1]
        v2 = this_element.vertices[i2]
        v3 = this_element.vertices[i3]

        x1 = v2.x - v1.x
        x2 = v3.x - v1.x
        y1 = v2.y - v1.y
        y2 = v3.y - v1.y
        z1 = v2.z - v1.z
        z2 = v3.z - v1.z

        s1 = v2.u - v1.u
        s2 = v3.u - v1.u

        t1 = v2.v - v1.v
        t2 = v3.v - v1.v

        den = (s1 * t2 - s2 * t1)
        if den == 0:
            return
        r = 1.0 / den
        sdirx = (t2*x1 - t1*x2)*r
        sdiry = (t2*y1 - t1*y2)*r
        sdirz = (t2*z1 - t1*z2)*r

        tdirx = (s2*x1 - s1*x2)*r
        tdiry = (s2*y1 - s1*y2)*r
        tdirz = (s2*z1 - s1*z2)*r

        v1.tan1x += sdirx
        v1.tan1y += sdiry
        v1.tan1z += sdirz
        v2.tan1x += sdirx
        v2.tan1y += sdiry
        v2.tan1z += sdirz
        v3.tan1x += sdirx
        v3.tan1y += sdiry
        v3.tan1z += sdirz

        v1.tan2x += tdirx
        v1.tan2y += tdiry
        v1.tan2z += tdirz
        v2.tan2x += tdirx
        v2.tan2y += tdiry
        v2.tan2z += tdirz
        v3.tan2x += tdirx
        v3.tan2y += tdiry
        v3.tan2z += tdirz

    with open(filename, "r") as f:
        for l in f.readlines():
            pos_match = pos_re.match(l)
            tc_match = tc_re.match(l)
            norm_match = norm_re.match(l)
            obj_match = obj_re.match(l)
            tri_face_match = tri_face_re.match(l)
            quad_face_match = quad_face_re.match(l)
            mtl_match = mtl_re.match(l)
            usemtr_match = usemtl_re.match(l)

            if mtl_match:
                all_materials.update(materials.parse_material_file(os.path.join(
                    os.path.dirname(filename), mtl_match.group(1)), bc, not ignore_textures, fast_encode))
            elif pos_match:
                pos = position()
                pos.x = float(pos_match.group(1))
                pos.y = float(pos_match.group(2))
                pos.z = float(pos_match.group(3))
                positions.append(pos)
            elif tc_match:
                tc = texcoord()
                tc.u = float(tc_match.group(1))
                tc.v = float(tc_match.group(2))
                texcoords.append(tc)
            elif norm_match:
                norm = normal()
                norm.nx = float(norm_match.group(1))
                norm.ny = float(norm_match.group(2))
                norm.nz = float(norm_match.group(3))
                normals.append(norm)
            elif obj_match:
                if this_element:
                    all_elements.append(this_element)
                    total_vertex_size += len(this_element.vertices)
                    total_index_size += len(this_element.indices)
                    total_name_size += len(this_element.name)

                    this_element.diffuse_offset = total_name_size
                    total_name_size += len(this_element.diffuse)
                    this_element.spec_offset = total_name_size
                    total_name_size += len(this_element.spec)
                    this_element.mask_offset = total_name_size
                    total_name_size += len(this_element.mask)
                    this_element.norm_offset = total_name_size
                    total_name_size += len(this_element.norm)

                    this_element.reset_origin()

                this_element = element()
                this_element.name = obj_match.group(1)
                this_element.vert_offset = total_vertex_size
                this_element.index_offset = total_index_size
                this_element.name_offset = total_name_size
                current_vertices = {}
            elif usemtr_match:
                material = all_materials[usemtr_match.group(1)]
                if len(this_element.vertices) > 0:
                    last_element_name = this_element.name
                    all_elements.append(this_element)
                    total_vertex_size += len(this_element.vertices)
                    total_index_size += len(this_element.indices)
                    total_name_size += len(this_element.name)

                    this_element.diffuse_offset = total_name_size
                    total_name_size += len(this_element.diffuse)
                    this_element.spec_offset = total_name_size
                    total_name_size += len(this_element.spec)
                    this_element.mask_offset = total_name_size
                    total_name_size += len(this_element.mask)
                    this_element.norm_offset = total_name_size
                    total_name_size += len(this_element.norm)

                    this_element.reset_origin()
                    this_element = element()
                    this_element.name = last_element_name + \
                        "_" + usemtr_match.group(1)
                    this_element.vert_offset = total_vertex_size
                    this_element.index_offset = total_index_size
                    this_element.name_offset = total_name_size
                    current_vertices = {}
                this_element.diffuse = material.diffuse
                this_element.spec = material.spec
                this_element.mask = material.mask
                this_element.norm = material.norm
            elif quad_face_match:
                indices = [
                    get_vertex(vertex_re.match(quad_face_match.group(1))),
                    get_vertex(vertex_re.match(quad_face_match.group(2))),
                    get_vertex(vertex_re.match(quad_face_match.group(3))),
                    get_vertex(vertex_re.match(quad_face_match.group(4)))
                ]
                this_element.indices.append(indices[0])
                this_element.indices.append(indices[1])
                this_element.indices.append(indices[2])
                this_element.indices.append(indices[0])
                this_element.indices.append(indices[2])
                this_element.indices.append(indices[3])
                process_triangle(indices[0], indices[1], indices[2])
                process_triangle(indices[0], indices[2], indices[3])

            elif tri_face_match:
                indices = [
                    get_vertex(vertex_re.match(tri_face_match.group(1))),
                    get_vertex(vertex_re.match(tri_face_match.group(2))),
                    get_vertex(vertex_re.match(tri_face_match.group(3)))
                ]
                this_element.indices.append(indices[0])
                this_element.indices.append(indices[1])
                this_element.indices.append(indices[2])
                process_triangle(indices[0], indices[1], indices[2])

    if this_element:
        all_elements.append(this_element)
        total_vertex_size += len(this_element.vertices)
        total_index_size += len(this_element.indices)
        total_name_size += len(this_element.name)

        this_element.diffuse_offset = total_name_size
        total_name_size += len(this_element.diffuse)
        this_element.spec_offset = total_name_size
        total_name_size += len(this_element.spec)
        this_element.mask_offset = total_name_size
        total_name_size += len(this_element.mask)
        this_element.norm_offset = total_name_size
        total_name_size += len(this_element.norm)
        this_element.reset_origin()

    NAMES_START = 4 + (OBJECT_HEADER_LENGTH) * len(all_elements)
    VERTEX_START = NAMES_START + total_name_size
    UNALIGNED_VERTEX_START = VERTEX_START
    VERTEX_START = (VERTEX_START + 3) & ~3

    INDEX_START = VERTEX_START + total_vertex_size * (VERTEX_LENGTH)

    # Fixup the offsets, now they are offsets into individual
    # lists, switch that to be offsets into the file
    for e in all_elements:
        e.name_offset = NAMES_START + e.name_offset
        e.vert_offset = VERTEX_START + e.vert_offset * VERTEX_LENGTH
        e.index_offset = INDEX_START + e.index_offset * INDEX_SIZE
        e.diffuse_offset += NAMES_START
        e.mask_offset += NAMES_START
        e.spec_offset += NAMES_START
        e.norm_offset += NAMES_START
    name_offset = []
    ct = 0

    with open(output_file, "wb") as of:
        o = array.array("L")
        o.append(len(all_elements))
        o.tofile(of)
        # Print out headers
        for e in all_elements:
            o = array.array("L",
                            [
                                e.name_offset,
                                len(e.name),
                                e.vert_offset,
                                len(e.vertices),
                                e.index_offset,
                                len(e.indices),
                                e.diffuse_offset,
                                len(e.diffuse),
                                e.spec_offset,
                                len(e.spec),
                                e.mask_offset,
                                len(e.mask),
                                e.norm_offset,
                                len(e.norm)
                            ]
                            )
            o.tofile(of)
            o = array.array("f",
                            [e.pos.x, e.pos.y, e.pos.z, 0.0])
            o.tofile(of)

        # Output all names
        for e in all_elements:
            array.array("c", e.name).tofile(of)
            array.array("c", e.diffuse).tofile(of)
            array.array("c", e.spec).tofile(of)
            array.array("c", e.mask).tofile(of)
            array.array("c", e.norm).tofile(of)

        # Align to 4 bytes
        for i in range(VERTEX_START - UNALIGNED_VERTEX_START):
            o = array.array("c", '\0')
            o.tofile(of)

        for e in all_elements:
            for v in e.vertices:
                o = array.array("f",
                                [v.x,
                                 v.y,
                                 v.z,
                                 v.nx,
                                 v.ny,
                                 v.nz,
                                 v.u,
                                 v.v,
                                 v.tanx,
                                 v.tany,
                                 v.tanz,
                                 v.tanw]
                                )
                o.tofile(of)
        for e in all_elements:
            o = array.array("L", e.indices)
            o.tofile(of)
