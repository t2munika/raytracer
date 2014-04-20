stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)
stone:set_normal_map("textures/sample_normal_map.png")
red = gr.material({1.0, 0.0, 0.0}, {0.3, 0.3, 0.3}, 20)
red:set_bump_map("textures/sample_bump_map.png")

scene = gr.node('root')

b1 = gr.cube('b1')
b1:translate(-2.5, 0, -10)
b1:scale(3, 3, 3)
b1:rotate('z', 30)
b1:rotate('y', 45)
b1:rotate('x', 45)
b1:set_material(stone)

b2 = gr.cube('b2')
b2:translate(2.5, 0, -10)
b2:scale(3, 3, 3)
b2:rotate('z', 30)
b2:rotate('y', 45)
b2:rotate('x', 45)
b2:set_material(red)

scene:add_child(b1)
scene:add_child(b2)

l1 = gr.light({0, 0, 0}, {0.8, 0.8, 0.8}, {1, 0, 0})
l3 = gr.light({0, 50, 0}, {0.7, 0.7, 0.7}, {1, 0, 0})

gr.render(scene, 'test_bump.png', 300, 300,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {l1, l3})
