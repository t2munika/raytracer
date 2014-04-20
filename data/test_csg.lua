require('room')

stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.0, 0.0, 0.0}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
blue = gr.material({0.4, 0.6, 1}, {0.0, 0.0, 0.0}, 25)
--blue = gr.material({0.0, 0.0, 0.0}, {0.9, 0.9, 0.9}, 25)
hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 20)
stone:set_normal_map("sample_normal_map.png")
tex = gr.texture("earth.png")
scene = gr.node('root')
scene:add_child(room)

csg1 = gr.difference('csg')
csg2 = gr.intersect('intersect')
csg3 = gr.union('union')

s1 = gr.sphere('s1')
s1:translate(0, 0, -10)
s1:set_material(mat3)
csg1:add_child(s1)

s2 = gr.sphere('s2')
s2:translate(1, 0, -9)
s2:set_material(mat3)
csg1:add_child(s2)

s3 = gr.nh_sphere('s3', {0, 2.5, -10}, 1)
s3:set_material(mat3)
csg2:add_child(s3)

s4 = gr.nh_sphere('s4', {1, 2.5, -9}, 1)
s4:set_material(mat3)
csg2:add_child(s4)

s5 = gr.nh_sphere('s5', {0, -2.5, -10}, 1)
s5:set_material(mat3)
csg3:add_child(s5)

s6 = gr.nh_sphere('s6', {1, -2.5, -9}, 1)
s6:set_material(mat3)
csg3:add_child(s6)

scene:add_child(csg1)
scene:add_child(csg2)
scene:add_child(csg3)

l1 = gr.light({0, 5, 5}, {1.0, 1.0, 1.0}, {1, 0, 0})

gr.render(scene, 'test_csg.png', 300, 300,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {l1})
