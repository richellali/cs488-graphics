
red = gr.material({0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, 25, 1, 1.2, '', '')

scene = gr.node('scene')
box_mat = gr.material_lib('cornellbox-water2.mtl')

box = gr.blender_mesh( 'box', 'cornell.obj' )
box:scale(10, 10, 10)
box:translate(0, -10, 0)
box:set_material_lib(box_mat)

s1 = gr.nh_sphere('s1', {0, 0.3, 0}, 0.3)
s1:set_material(red)

box:add_child(s1)

scene:add_child(box)

gr.render(scene,
	  'test.png', 512, 512,
	  {0, 0, 30}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {gr.light({0, 1, 430}, {0.8, 0.8, 0.8}, {1, 0, 0})})