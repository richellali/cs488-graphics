-- A simple test scene featuring some spherical cows grazing
-- around Stonehenge.  "Assume that cows are spheres..."

-- stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)
-- grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
-- hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 20)

table = gr.material({0.8, 0.66, 0.5}, {0.0, 0.0, 0.0}, 0, 1, -1.0, '', '')
red = gr.material({0.7, 0.2, 0.1}, {0.3, 0.3, 0.3}, 0, 1, -1, '', '')
transparency = gr.material({0, 0, 0}, {1.0, 1.0, 1.0}, 0, 1, 1.4, '', '')


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
-- plane:translate(0, 7, 0)

-- Construct a central altar in the shape of a buckyball.  The
-- buckyball at the centre of the real Stonehenge was destroyed
-- in the great fiEmpty Materialre of 733 AD.

-- s2 = gr.nh_sphere('s2', {200, 50, -100}, 150)
-- scene_root:add_child(s2)
-- s2:set_material(mat4)

s1 = gr.nh_sphere('s1', {-3, 3, 6}, 3)
scene:add_child(s1)
s1:set_material(red)

-- s2 = gr.nh_sphere('s2', {-3, 3, 6}, -2.9)
-- scene:add_child(s2)
-- s2:set_material(transparency)

c1 = gr.nh_cylinder('c1', {1, 3, 0}, 3, 6)
c1:set_material(red)

cy_ins = gr.node('cylinder')
cy_ins:rotate('Y', 90)
scene:add_child(cy_ins)
cy_ins:add_child(c1)

cone = gr.nh_cone('cone', {2, 2, 4}, 3, 6)
cone:set_material(red)
cone_ins = gr.node('cone_ins')
scene:add_child(cone_ins)
cone_ins:rotate('Y', 15)
cone_ins:add_child(cone)

gr.render(scene,
	  'with-soft-shadow.png', 1024, 1024,
	  {0, 2, 30}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, 
	  {
		gr.light({20, 20, 20}, {0.8, 0.8, 0.8}, {1, 0, 0})
		-- gr.light('l1.obj')
	})
