require('room')

stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
blue = gr.material({0.4, 0.6, 1}, {0.0, 0.0, 0.0}, 0)
hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 20)
red = gr.material({1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 0)

scene = gr.node('root')
scene:add_child(room)

s1 = gr.nh_sphere('s1', {-2, -2, -5}, 1)
s1:set_material(red)
scene:add_child(s1)


s2 = gr.nh_sphere('s2', {-1, -1, -7.5}, 1)
s2:set_material(blue)
scene:add_child(s2)

s3 = gr.nh_sphere('s3', {0, 0, -10}, 1)
s3:set_material(stone)
scene:add_child(s3)

s4 = gr.nh_sphere('s4', {1, 1, -12.5}, 1)
s4:set_material(blue)
scene:add_child(s4)

s5 = gr.nh_sphere('s5', {2, 2, -15}, 1)
s5:set_material(red)
scene:add_child(s5)

l1 = gr.light({0, 3, 10}, {0.5, 0.5, 0.5}, {1, 0, 0})
l2 = gr.light({0, 3, -20}, {0.5, 0.5, 0.5}, {1, 0, 0})

gr.render(scene, 'test.png', 300, 300,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {l1, l2})
