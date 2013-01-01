
# Tweakable physical parameters
wall_thickness = 3.5
base_thickness = 3.5
screen_thickness = 3.5
frame_thickness = 3.5
cell_pitch = 30.0
cells_horiz = 24  # must be at least 6, or adjust the power/mic hole locations
cells_vert = 7
height = 20.0
ledstrip_thickness = 4.0
ledstrip_width = 18.0
mitre_width = 9.0
mitre_tolerance = 0.5
part_padding = 10.0
fastener_bracket_curve_radius = 2
fastener_tab_thickness = 3.5
fastener_tab_width = 11.0
fastener_tab_curve_radius = 2
fastener_bracket_thickness = (cell_pitch - fastener_tab_width - wall_thickness) / 2
fastener_mitre_width = fastener_tab_width + (fastener_bracket_thickness * 2)
power_jack_diameter = 13.5
mic_diameter = 10.0
mitre_hole_width = mitre_width + mitre_tolerance
fastener_mitre_hole_width = fastener_mitre_width + mitre_tolerance
fastener_to_mitre_curve_radius = 1.0
frame_lip = 10.0
frame_lip_curve_radius = 5.0

# Two of the horizontal cells are special in that they poke through the base
# further to form the mechanism by which the grid fastens to the
# base such that it can be removed later for maintenence.
fastener_cell_idx = int(cells_horiz * 0.25)
fastener_cells = [
    fastener_cell_idx,
    cells_horiz - fastener_cell_idx - 1,
]

from cadoodle import Drawing
from math import ceil, floor

d = Drawing()

vert = d.add_layer("VERT")
horiz = d.add_layer("HORIZ")
vert_edge = d.add_layer("VERT_EDGE")
horiz_edge_bottom = d.add_layer("HORIZ_EDGE_BOTTOM")
horiz_edge_top = d.add_layer("HORIZ_EDGE_TOP")
base = d.add_layer("BASE")
screen = d.add_layer("SCREEN")
fasten_tab = d.add_layer("FASTEN_TAB")
frame = d.add_layer("FRAME")

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
    vert.move(0, base_thickness)
    vert.east(wall_thickness / 2.0) # extra "lip" for the outer wall
    for i in xrange(0, cells_vert + 1):
        # complete the mitre
        vert.east(mitre_width / 2.0)
        if i != 0:
            vert.north(base_thickness)
        vert.east(cell_flush_bottom / 2.0)
        vert.north(ledstrip_thickness)
        vert.east(ledstrip_width)
        vert.south(ledstrip_thickness)
        vert.east(cell_flush_bottom / 2.0)
        if i != cells_vert:
            vert.south(base_thickness)
        # start the next mitre
        vert.east(mitre_width / 2.0)
    vert.east(wall_thickness / 2.0) # extra "lip" for the outer wall

    # East edge
    vert.north(height / 2.0)

    # Top edge
    vert.west(wall_thickness / 2.0) # extra "lip" for the outer wall
    for i in xrange(0, cells_vert + 1):
        # complete the slit for the perpendicular walls
        vert.west(wall_thickness / 2.0)
        vert.north(height / 2.0)
        vert.west(cell_flush_top)
        vert.south(height / 2.0)
        # start the next slit
        vert.west(wall_thickness / 2.0)
    vert.west(wall_thickness / 2.0) # extra "lip" for the outer wall

    # West edge
    vert.south(height / 2.0)


def draw_vert_edge(part):

    cell_flush_bottom = cell_pitch - mitre_width
    cell_flush_top = (
        cell_pitch
        - mitre_width
        - wall_thickness
    )

    # bottom edge
    part.move(0, base_thickness)
    part.east(wall_thickness / 2.0)
    for i in xrange(0, cells_vert + 2):
        part.east(cell_flush_bottom / 2.0)
        part.south(base_thickness)
        part.east(mitre_width)
        part.north(base_thickness)
        part.east(cell_flush_bottom / 2.0)
    part.east(wall_thickness / 2.0)

    # East edge
    part.north(height / 2.0)

    # Top edge
    part.west(wall_thickness / 2.0)
    for i in xrange(0, cells_vert + 2):
        part.west(wall_thickness / 2.0)
        part.north(height / 2.0)
        part.west(cell_flush_top / 2.0)
        part.north(screen_thickness + frame_thickness)
        part.west(mitre_width)
        part.south(screen_thickness + frame_thickness)
        part.west(cell_flush_top / 2.0)
        part.south(height / 2.0)
        part.west(wall_thickness / 2.0)
    part.west(wall_thickness / 2.0)

    # West edge
    part.south(height / 2.0)


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
                fastener_mitre_width - (fastener_bracket_curve_radius * 2)
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


def draw_horiz_edge(part, mod, holes=False):
    cell_flush_bottom = (
        cell_pitch
        - wall_thickness # slit for the perpendicular wall
        - mitre_width    # mitre to mate with the base
    )
    cell_flush_top = (
        cell_pitch
        - mitre_width
    )

    part.move(0, (height / 2.0) + base_thickness)

    part.east(wall_thickness / 2.0) # extra "lip" for the outer wall
    end_up = True
    for i in xrange(0, cells_horiz + 2):
        # complete the slit for the perpendicular walls
        part.east(wall_thickness / 2.0)
        if end_up:
            part.south(height / 2.0)
        part.east(cell_flush_bottom / 2.0)
        part.south(base_thickness)
        part.east(mitre_width)
        part.north(base_thickness)
        part.east(cell_flush_bottom / 2.0)
        if (i % 2 == mod) or (i == cells_horiz + 1):
            part.north(height / 2.0)
            end_up = True
        else:
            end_up = False
        part.east(wall_thickness / 2.0)
    part.east(wall_thickness / 2.0) # extra "lip" for the outer wall

    # East edge
    part.north(height / 2.0)

    # Top edge.
    part.west(wall_thickness / 2.0)
    for i in xrange(0, cells_horiz + 2):
        part.west(cell_flush_top / 2.0)
        part.north(screen_thickness + frame_thickness)
        part.west(mitre_width)
        part.south(screen_thickness + frame_thickness)
        part.west(cell_flush_top / 2.0)
    part.west(wall_thickness / 2.0)

    # West edge
    part.south(height / 2.0)

    # Power jack hole
    if holes:
        part.move((cell_pitch * 6.0) + (wall_thickness / 2.0), (power_jack_diameter / 2.0))
        part.curve_ne_cw(power_jack_diameter / 2.0)
        part.curve_se_cw(power_jack_diameter / 2.0)
        part.curve_sw_cw(power_jack_diameter / 2.0)
        part.curve_nw_cw(power_jack_diameter / 2.0)
        part.move(0, -power_jack_diameter / 2.0)
        part.move(-cell_pitch * 4.0, mic_diameter / 2.0)
        part.curve_ne_cw(mic_diameter / 2.0)
        part.curve_se_cw(mic_diameter / 2.0)
        part.curve_sw_cw(mic_diameter / 2.0)
        part.curve_nw_cw(mic_diameter / 2.0)


def draw_screen(part):
    between_hole_length = cell_pitch - mitre_hole_width

    counts_dirs = [
        (part.east, part.north, cells_horiz + 2),
        (part.north, part.west, cells_vert + 2),
        (part.west, part.south, cells_horiz + 2),
        (part.south, part.east, cells_vert + 2),
    ]

    for along, inward, count in counts_dirs:
        along(wall_thickness / 2.0)
        for i in xrange(0, count):
            along(between_hole_length / 2.0)
            inward(wall_thickness)
            along(mitre_hole_width)
            inward(-wall_thickness)
            along(between_hole_length / 2.0)
        along(wall_thickness / 2.0)


def draw_base(part):
    # It's easier to be able to return back to the origin
    # for each iteration.
    origin_x = part.x
    origin_y = part.y

    # The base starts off being the same as the screen and then
    # has some extra holes cut in it for the wall mitres.
    draw_screen(part)

    # Since the fastener hole intersects with the vertical mitre holes
    # we want to cut it to fit between the two adjacent holes rather than
    # explicitly cutting a tolerance like we would elsewhere.
    effective_fastener_hole_width = cell_pitch - wall_thickness

    along_offset = (cell_pitch - mitre_hole_width) / 2.0
    aside_offset = cell_pitch - (wall_thickness / 2.0)
    fastener_along_offset = (cell_pitch - effective_fastener_hole_width) / 2.0

    for ni in xrange(0, cells_vert + 1):
        for ei in xrange(0, cells_horiz):
            is_fastener = ei in fastener_cells
            part.x = origin_x + (wall_thickness / 2.0) + (cell_pitch * (ei + 1.0))
            part.y = origin_y + (wall_thickness / 2.0) + (cell_pitch * ni)

            if is_fastener:
                part.move(fastener_along_offset, aside_offset)
                part.move(0, -fastener_to_mitre_curve_radius)
                part.curve_nw_cw(fastener_to_mitre_curve_radius)
                part.east(effective_fastener_hole_width - (fastener_to_mitre_curve_radius * 2.0))
                part.curve_ne_cw(fastener_to_mitre_curve_radius)
                part.move(0, wall_thickness + fastener_to_mitre_curve_radius * 2)
                part.curve_se_cw(fastener_to_mitre_curve_radius)
                part.west(effective_fastener_hole_width - (fastener_to_mitre_curve_radius * 2.0))
                part.curve_sw_cw(fastener_to_mitre_curve_radius)
                part.move(0, -wall_thickness)
            else:
                part.move(along_offset, aside_offset)
                part.east(mitre_hole_width)
                part.north(wall_thickness)
                part.west(mitre_hole_width)
                part.south(wall_thickness)

    fastener_side_space = wall_thickness + (fastener_to_mitre_curve_radius * 2)
    fastener_side_cut = mitre_hole_width - fastener_side_space

    for ei in xrange(0, cells_horiz + 1):
        for ni in xrange(0, cells_vert + 1):
            is_fastener_left = ei in fastener_cells
            is_fastener_right = ei - 1 in fastener_cells
            part.x = origin_x + (wall_thickness / 2.0) + (cell_pitch * ei)
            part.y = origin_y + (wall_thickness / 2.0) + (cell_pitch * ni)

            part.move(
                cell_pitch - (wall_thickness / 2.0),
                cell_pitch - (mitre_hole_width / 2.0),
            )

            if is_fastener_right:
                part.north(fastener_side_cut / 2.0)
                part.move(0, fastener_side_space)
                part.north(fastener_side_cut / 2.0)
            else:
                part.north(mitre_hole_width)
            part.east(wall_thickness)
            if is_fastener_left:
                part.south(fastener_side_cut / 2.0)
                part.move(0, -fastener_side_space)
                part.south(fastener_side_cut / 2.0)
            else:
                part.south(mitre_hole_width)
            part.west(wall_thickness)


def draw_fasten_tab(part):

    fastener_tab_handle_height = cell_pitch * 0.75
    fastener_tab_handle_width = fastener_tab_width * 3.0
    fastener_tab_length = (cell_pitch * cells_vert) + fastener_tab_curve_radius

    part.move(0, fastener_tab_curve_radius)
    part.curve_sw_ccw(fastener_tab_curve_radius)
    part.east(fastener_tab_handle_height - (fastener_tab_curve_radius * 2.0))
    part.curve_se_ccw(fastener_tab_curve_radius)
    part.north(((fastener_tab_handle_width - fastener_tab_width) / 2.0) - fastener_tab_curve_radius)
    part.east(fastener_tab_length)
    part.curve_se_ccw(fastener_tab_curve_radius)
    part.north(fastener_tab_width - (fastener_tab_curve_radius * 2.0) - mitre_tolerance)
    part.curve_ne_ccw(fastener_tab_curve_radius)
    part.west(fastener_tab_length)
    part.north(((fastener_tab_handle_width - fastener_tab_width) / 2.0) - fastener_tab_curve_radius)
    part.curve_ne_ccw(fastener_tab_curve_radius)
    part.west(fastener_tab_handle_height - (fastener_tab_curve_radius * 2.0))
    part.curve_nw_ccw(fastener_tab_curve_radius)
    part.south(fastener_tab_handle_width - (fastener_tab_curve_radius * 2.0) - mitre_tolerance)


def draw_frame(part):
    origin_x = part.x
    origin_y = part.y

    straight_across_outer = ((cells_horiz + 2.0) * cell_pitch) + (frame_lip * 2.0) - (frame_lip_curve_radius * 2.0) + wall_thickness
    straight_down_outer = ((cells_vert + 2.0) * cell_pitch) + (frame_lip * 2.0) - (frame_lip_curve_radius * 2.0) + wall_thickness
    straight_across_inner = (cells_horiz * cell_pitch) - (frame_lip_curve_radius * 2)
    straight_down_inner = (cells_vert * cell_pitch) - (frame_lip_curve_radius * 2.0)

    part.move(0, frame_lip_curve_radius)
    part.curve_sw_ccw(frame_lip_curve_radius)
    part.east(straight_across_outer)
    part.curve_se_ccw(frame_lip_curve_radius)
    part.north(straight_down_outer)
    part.curve_ne_ccw(frame_lip_curve_radius)
    part.west(straight_across_outer)
    part.curve_nw_ccw(frame_lip_curve_radius)
    part.south(straight_down_outer)

    part.x = origin_x
    part.y = origin_y

    part.move(
        frame_lip + cell_pitch + (wall_thickness / 2.0),
        frame_lip + cell_pitch + (wall_thickness / 2.0),
    )
    part.move(0, frame_lip_curve_radius)
    part.curve_sw_ccw(frame_lip_curve_radius)
    part.east(straight_across_inner)
    part.curve_se_ccw(frame_lip_curve_radius)
    part.north(straight_down_inner)
    part.curve_ne_ccw(frame_lip_curve_radius)
    part.west(straight_across_inner)
    part.curve_nw_ccw(frame_lip_curve_radius)
    part.south(straight_down_inner)

    between_hole_length = cell_pitch - mitre_hole_width

    part.x = origin_x + frame_lip
    part.y = origin_y + frame_lip

    counts_dirs = [
        (part.east, part.north, cells_horiz + 2),
        (part.north, part.west, cells_vert + 2),
        (part.west, part.south, cells_horiz + 2),
        (part.south, part.east, cells_vert + 2),
    ]

    for along, inward, count in counts_dirs:
        part.pen_up()
        along(wall_thickness / 2.0)
        for i in xrange(0, count):
            along(between_hole_length / 2.0)
            part.pen_down()
            inward(wall_thickness)
            along(mitre_hole_width)
            inward(-wall_thickness)
            along(-mitre_hole_width)
            part.pen_up()
            along((between_hole_length / 2.0) + mitre_hole_width)
        along(wall_thickness / 2.0)
        part.pen_down()


vert.x = part_padding
vert.y = part_padding
draw_vert(vert)

vert_edge.x = part_padding
vert_edge.y = part_padding * 2 + height + base_thickness
draw_vert_edge(vert_edge)

horiz.x = part_padding
horiz.y = (part_padding * 3) + (height * 2) + (base_thickness * 2) + screen_thickness + frame_thickness + fastener_tab_thickness + fastener_bracket_thickness
draw_horiz(horiz)

horiz_edge_bottom.x = part_padding
horiz_edge_bottom.y = (part_padding * 4) + (height * 3) + (base_thickness * 3) + screen_thickness + frame_thickness + fastener_tab_thickness + fastener_bracket_thickness
draw_horiz_edge(horiz_edge_bottom, 0, True)

horiz_edge_top.x = part_padding
horiz_edge_top.y = (part_padding * 5) + (height * 4) + (base_thickness * 4) + (screen_thickness * 2) + (frame_thickness * 2) + fastener_tab_thickness + fastener_bracket_thickness
draw_horiz_edge(horiz_edge_top, 1, False)

base.x = part_padding
base.y = (part_padding * 6) + (height * 5) + (base_thickness * 5) + (screen_thickness * 3) + (frame_thickness * 3) + fastener_tab_thickness + fastener_bracket_thickness
draw_base(base)

screen.x = part_padding
screen.y = (part_padding * 8) + (height * 5) + (base_thickness * 5) + (screen_thickness * 3) + (frame_thickness * 3) + fastener_tab_thickness + fastener_bracket_thickness + (cell_pitch * (cells_vert + 2))
draw_screen(screen)

fasten_tab.x = (part_padding * 4) + (wall_thickness) + (cell_pitch * (cells_vert + 2))
fasten_tab.y = part_padding
draw_fasten_tab(fasten_tab)

frame.x = part_padding
frame.y = (part_padding * 10) + (height * 5) + (base_thickness * 5) + (screen_thickness * 3) + (frame_thickness * 3) + fastener_tab_thickness + fastener_bracket_thickness + (cell_pitch * (cells_vert + 2) * 2)
draw_frame(frame)

d.save("schematic.dxf")
