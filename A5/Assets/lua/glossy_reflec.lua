-- A simple test scene featuring some spherical cows grazing
-- around Stonehenge.  "Assume that cows are spheres..."

-- stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)
-- grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
-- hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 20)

-- table = gr.material({0.8, 0.66, 0.5}, {0.0, 0.0, 0.0}, 0, 0.95, -1.0)
table = gr.material({1, 1, 1}, {0, 0, 0}, 0, 1, -1.0)
red = gr.material({0.7, 0.2, 0.1}, {0, 0, 0}, 0, 0.0, -1.0)


-- ##############################################
-- the scene
-- ##############################################

scene = gr.node('scene')
scene:rotate('X', 23)

-- the floor

plane = gr.mesh( 'plane', 'plane.obj' )
scene:add_child(plane)
plane:set_material(table)
plane:scale(30, 30, 30)

-- Construct a central altar in the shape of a buckyball.  The
-- buckyball at the centre of the real Stonehenge was destroyed
-- in the great fire of 733 AD.

-- s2 = gr.nh_sphere('s2', {200, 50, -100}, 150)
-- scene_root:add_child(s2)
-- s2:set_material(mat4)

s1 = gr.sphere('s1')
scene:add_child(s1)
s1:set_material(red)
s1:translate(-2, 1, 2)
s1:scale(3, 3, 3)

c1 = gr.nh_cylinder('c1', {1, 3, 0}, 3, 6)
c1:set_material(red)

cy_ins = gr.node('cylinder')
cy_ins:rotate('Y', 90)
scene:add_child(cy_ins)
cy_ins:add_child(c1)


gr.render(scene,
	  'test.png', 512, 512,
	  {0, 2, 30}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {gr.light({200, 202, 430}, {0.8, 0.8, 0.8}, {1, 0, 0})})
