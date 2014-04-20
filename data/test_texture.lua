require('room')

fleurdelis = gr.texture("textures/fleurdelis.png")
world = gr.texture("textures/earth.png")

scene = gr.node('root')
scene:add_child(room)

s1 = gr.nh_sphere('s1', {1, 1, -10}, 1.5)
s1:set_material(world)
scene:add_child(s1)

b1 = gr.cube('b1')
b1:translate(-1, -1, -10)
b1:scale(2, 2, 2)
b1:rotate('y', 20)
b1:set_material(fleurdelis)
scene:add_child(b1)

l1 = gr.light({0, 3, 10}, {0.5, 0.5, 0.5}, {1, 0, 0})
l2 = gr.light({0, 3, -20}, {0.5, 0.5, 0.5}, {1, 0, 0})

gr.render(scene, 'test_texture.png', 300, 300,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {l1, l2})
