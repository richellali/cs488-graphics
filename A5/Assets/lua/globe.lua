trans_glass_mat = gr.material({0, 0, 0}, {1, 1, 1}, 1000, 0, 1.5, '', '')
reflec_glass_mat = gr.material({1, 1, 1}, {0, 0, 0}, 1000, 1, -1, '', '')
glossy_glass_mat = gr.material({0.5, 0.5, 0.5}, {0.5, 0.5, 0.5}, 0, 0.99, 1.45, '', '')
glossy_reflec_mat = gr.material({0.5, 0.5, 0.5}, {0.5, 0.5, 0.5}, 0, 0.99, -1, '', '')
blue = gr.material({0.055, 0.3, 0.612}, {0.3, 0.3, 0.3}, 0, 1, -1, '', '')

star_mat = gr.material({1.0, 0.97, 0.1}, {0.5, 0.7, 0.5}, 25, 1, -1, '', '')
globe_mat = gr.material_lib('test.mtl')
-- table_mat = gr.material_lib('table.mtl')
-- cube_mat = gr.material_lib('cube.mtl')


-- ##############################################
-- the scene
-- ##############################################

scene = gr.node('scene')
-- scene:rotate('X', 23)

-- the floor

table_node = gr.node('table')
scene:add_child(table_node)
table_node:translate(0.1, -1.25, 0)
table_node:rotate('X', 13)
table_node:scale(15, 15, 15)

proj= gr.blender_mesh('proj', 'proj.obj')
table_node:add_child(proj)
proj:set_material_lib(globe_mat)
proj:rotate('Y', 120)


glass_out = gr.nh_sphere('globe1', {-0.04, 1.55, 0.1}, 0.4)
glass_out:set_material(trans_glass_mat)

glass_in = gr.nh_sphere('globe2', {-0.04, 1.55, 0.1}, -0.39)
glass_in:set_material(trans_glass_mat)

g_s = gr.node('g_s')
g_s:translate(0.4, 1.27, 0.5);

t1 = gr.nh_sphere('globe1', {0, 0, 0}, 0.15)
t1:set_material(glossy_glass_mat)
t2 = gr.nh_sphere('globe1', {0, 0, 0}, -0.14)
t2:set_material(glossy_glass_mat)

star = gr.mesh('star', 'estrellica.obj')
star:scale(0.015, 0.015, 0.015)
star:rotate('X', 90)
star:set_material(star_mat)

s1 = gr.nh_sphere('globe1', {0, 0, 0}, 0.1)
s1:set_material(trans_glass_mat)
s1:translate(0.05, -0.05, 0.25)

c1 = gr.nh_cylinder('cylin', {0.0, 0.0, 0.0}, 0.06, 0.15)
c1:set_material(blue)
c1:rotate('Y', 90)
c1:translate(-0.3, -0.05, 0.1)

-- cone = gr.nh_cone('cone', {0,0,0}, 0.1, 0.2)
-- cone:set_material(blue)
-- -- cone:rotate('Y', 15)
-- cone:rotate('X', 45)
-- cone:translate(3, -1, 25)

g_s:add_child(c1)
-- scene:add_child(cone)
g_s:add_child(t1)
g_s:add_child(t2)
g_s:add_child(star)
g_s:add_child(s1)


table_node:add_child(glass_in)
table_node:add_child(glass_out)
table_node:add_child(g_s)
table_node:add_child(t2)

gr.render(scene,
	  'test.png', 1024, 1024,
	  {0, 2, 30}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {gr.light({20, 20, 20}, {0.8, 0.8, 0.8}, {1, 0, 0})})
