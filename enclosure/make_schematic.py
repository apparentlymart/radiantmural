
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
part_padding = 2.0
fastener_bracket_thickness = 8.0
fastener_bracket_curve_radius = 2
fastener_tab_thickness = 3.0
fastener_tab_width = 20.0

from cadoodle import Drawing
from math import ceil, floor

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
    cell_flush_bottom_fastener = (
        cell_pitch
        - (fastener_bracket_thickness * 2)
        - fastener_tab_width
        - wall_thickness # slit for the perpendicular wall
    )

    horiz.move(0, (height / 2.0) + base_thickness)

    # Two of the cells are special in that they poke through the base
    # further to form the mechanism by which the grid fastens to the
    # base such that it can be removed later for maintenence.
    fastener_cell_idx = int(cells_horiz * 0.25)
    fastener_cells = [
        fastener_cell_idx,
        cells_horiz - fastener_cell_idx - 1,
    ]

    # Bottom edge
    horiz.east(wall_thickness / 2.0) # extra "lip" for the outer wall
    for i in xrange(0, cells_horiz):
        is_fastener = i in fastener_cells
        # complete the slit for the perpendicular walls
        horiz.east(wall_thickness / 2.0)
        horiz.south(height / 2.0)
        if is_fastener:
            horiz.east(cell_flush_bottom_fastener / 2.0)
            horiz.south(base_thickness + fastener_bracket_thickness + fastener_tab_thickness - fastener_bracket_curve_radius)
            horiz.curve_sw_ccw(fastener_bracket_curve_radius)
            horiz.east(
                fastener_tab_width
                + (fastener_bracket_thickness * 2)
                - (fastener_bracket_curve_radius * 2)
            )
            horiz.curve_se_ccw(fastener_bracket_curve_radius)
            horiz.north(base_thickness + fastener_bracket_thickness + fastener_tab_thickness - fastener_bracket_curve_radius)

            # Double back for a moment and draw the hole where the
            # fastener tab will go.
            horiz.move(
                -(fastener_bracket_thickness + fastener_tab_width),
                -base_thickness,
            )
            horiz.south(fastener_tab_thickness)
            horiz.east(fastener_tab_width)
            horiz.north(fastener_tab_thickness)
            horiz.west(fastener_tab_width)
            horiz.move(
                (fastener_bracket_thickness + fastener_tab_width),
                base_thickness,
            )

            horiz.east(cell_flush_bottom_fastener / 2.0)
        else:
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
horiz.y = (part_padding * 2) + height + base_thickness + fastener_tab_thickness + fastener_bracket_thickness
draw_horiz(horiz)

d.save("schematic.dxf")
