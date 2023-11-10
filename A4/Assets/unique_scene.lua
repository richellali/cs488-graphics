
-- A simple test scene featuring some spherical cows grazing
-- around Stonehenge.  "Assume that cows are spheres..."

stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 20)

darkgrass= gr.texture('dry_grass.png')
mercury_tex= gr.texture('mercury.png')
sun_tex= gr.texture('sun.png')
sand_tex = gr.texture('dryland.png')

-- ##############################################
-- the arch
-- ##############################################

-- inst = gr.node('inst')

arc = gr.node('arc')
-- inst:add_child(arc)
arc:translate(0, 0, -10)

p1 = gr.nh_box('p1', {0, 0, 0}, 1)
arc:add_child(p1)
p1:set_material(stone)
p1:translate(-2.4, 0, -0.4)
p1:scale(0.8, 4, 0.8)

p2 = gr.nh_box('p2', {0, 0, 0}, 1)
arc:add_child(p2)
p2:set_material(stone)
p2:translate(1.6, 0, -0.4)
p2:scale(0.8, 4, 0.8)

s = gr.nh_sphere('s', {0, 0, 0}, 1)
arc:add_child(s)
s:set_material(stone)
s:scale(4, 0.6, 0.6)
s:translate(0, 4, 0)

-- ##############################################
-- the box
-- ##############################################
box = gr.nh_box('b', {0,0,0}, 3)
box:set_material(hide)
box:set_texture(sand_tex)
box:translate(8, 0, -3)


-- #############################################
-- Let's assume that cows are spheres
-- #############################################

cow_poly = gr.mesh('cow', 'cow.obj')
factor = 2.0/(2.76+3.637)

cow_poly:set_material(hide)
cow_poly:translate(0.0, 3.637, 0.0)
cow_poly:scale(factor, factor, factor)
cow_poly:translate(0.0, -1.0, 0.0)

-- ##############################################
-- the scene
-- ##############################################


scene = gr.node('scene')
scene:rotate('X', 23)

-- the floor

plane = gr.mesh( 'plane', 'plane.obj' )
scene:add_child(plane)
plane:set_material(grass)
plane:scale(30, 30, 30)
plane:set_texture(darkgrass)

-- Construct a central altar in the shape of a buckyball.  The
-- buckyball at the centre of the real Stonehenge was destroyed
-- in the great fire of 733 AD.

buckyball = gr.nh_sphere( 'buckyball',  {0, 0, 0}, 3)
scene:add_child(buckyball)
buckyball:set_material(stone)
buckyball:set_texture(mercury_tex)
buckyball:translate(0.0, 1.0, 0.0)


sun = gr.nh_sphere( 'sun',  {0, 0, 0}, 5)
scene:add_child(sun)
sun:set_material(stone)
sun:set_texture(sun_tex)
sun:translate(12.0, 12.0, -30.0)

-- Use the instanced cow model to place some actual cows in the scene.
-- For convenience, do this in a loop.

cow_number = 1

for _, pt in pairs({
		      {{1,1.3,14}, 20},
		      {{5,1.3,-11}, 180}}) do
   cow_instance = gr.node('cow' .. tostring(cow_number))
   scene:add_child(cow_instance)
   cow_instance:add_child(cow_poly)
   cow_instance:scale(1.4, 1.4, 1.4)
   cow_instance:rotate('Y', pt[2])
   cow_instance:translate(table.unpack(pt[1]))
   
   cow_number = cow_number + 1
end

-- Place a ring of arches.

for i = 1, 3 do
   a_box = gr.node('box' .. tostring(i))
   a_box:translate(0, (i-1)*3.0, 0)
   scene:add_child(a_box)
   a_box:add_child(box)
end

an_arc = gr.node('arc1')
an_arc:rotate('Y', 1 * 60)
scene:add_child(an_arc)
an_arc:add_child(arc)

gr.render(scene,
	  'unique_scene.png', 512, 512,
	  {0, 2, 30}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {gr.light( {200, 202, -100}, {0.8, 0.7, 0.4}, {1, 0, 0})})
-- {200, 202, -100},