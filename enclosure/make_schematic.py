
# Tweakable physical parameters
wall_thickness = 3.0
base_thickness = 2.0
screen_thickness = 1.0
cell_pitch = 50.0
cells_horiz = 10
cells_vert = 7
height = 12.0
ledstrip_thickness = 2.0
ledstrip_width = 10.0
mitre_width = 10.0
part_padding = 2

from cadoodle import Drawing

d = Drawing()

vert = d.add_layer("VERT")
horiz = d.add_layer("HORIZ")

def draw_vert(vert):
    cell_flush_bottom = (
        cell_pitch
        - ledstrip_width   # slit for the LED strip to pass under
        - mitre_width      # mitre to mate with the base
    )
    cell_flush_top = (
        cell_pitch
        - wall_thickness   # slit for the perpendicular wall
    )

    # Bottom edge
    vert.east(wall_thickness / 2.0) # extra "lip" for the outer wall
    for i in xrange(0, cells_vert):
        # complete the mitre
        vert.east(mitre_width / 2.0)
        vert.north(base_thickness)
        vert.east(cell_flush_bottom / 2.0)
        vert.north(ledstrip_thickness)
        vert.east(ledstrip_width)
        vert.south(ledstrip_thickness)
        vert.east(cell_flush_bottom / 2.0)
        vert.south(base_thickness)
        # start the next mitre
        vert.east(mitre_width / 2.0)
    vert.east(wall_thickness / 2.0) # extra "lip" for the outer wall

    # East edge
    vert.north((height / 2.0) + base_thickness)

    # Top edge
    vert.west(wall_thickness / 2.0) # extra "lip" for the outer wall
    for i in xrange(0, cells_vert):
        # complete the slit for the perpendicular walls
        vert.west(wall_thickness / 2.0)
        vert.north(height / 2.0)
        vert.west(cell_flush_top)
        vert.south(height / 2.0)
        # start the next slit
        vert.west(wall_thickness / 2.0)
    vert.west(wall_thickness / 2.0) # extra "lip" for the outer wall

    # West edge
    vert.south((height / 2.0) + base_thickness)

vert.x = part_padding
vert.y = part_padding
draw_vert(vert)

def draw_horiz(horiz):
    cell_flush_bottom = (
        cell_pitch
        - wall_thickness # slit for the perpendicular wall
        - mitre_width    # mitre to mate with the base
    )

    horiz.move(0, (height / 2.0) + base_thickness)

    # Bottom edge
    horiz.east(wall_thickness / 2.0) # extra "lip" for the outer wall
    for i in xrange(0, cells_horiz):
        # complete the slit for the perpendicular walls
        horiz.east(wall_thickness / 2.0)
        horiz.south(height / 2.0)
        horiz.east(cell_flush_bottom / 2.0)
        horiz.south(base_thickness)
        horiz.east(mitre_width)
        horiz.north(base_thickness)
        horiz.east(cell_flush_bottom / 2.0)
        horiz.north(height / 2.0)
        horiz.east(wall_thickness / 2.0)
    horiz.east(wall_thickness / 2.0) # extra "lip" for the outer wall

    # East edge
    horiz.north(height / 2.0)

    # Top edge. This one's easy!
    horiz.west((cell_pitch * cells_horiz) + wall_thickness)

    # West edge
    horiz.south(height / 2.0)

horiz.x = part_padding
horiz.y = (part_padding * 2) + height + base_thickness
draw_horiz(horiz)

d.save("schematic.dxf")
