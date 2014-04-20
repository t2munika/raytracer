require('room')

stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
blue = gr.material({0.4, 0.6, 1}, {0.9, 0.9, 0.9}, 25)
hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 20)
red = gr.material({1.0, 0.0, 0.0}, {0.3, 0.3, 0.3}, 20)

scene = gr.node('root')
scene:add_child(room)

c1 = gr.cylinder('c1')
c1:translate(2, 0, -10)
c1:set_material(blue)
scene:add_child(c1)

c2 = gr.cone('c2')
c2:translate(-2, -1, -10)
c2:scale(1, 2, 1)
c2:set_material(red)
scene:add_child(c2)

l1 = gr.light({0, 3, 10}, {0.5, 0.5, 0.5}, {1, 0, 0})
l2 = gr.light({0, 3, -20}, {0.5, 0.5, 0.5}, {1, 0, 0})

gr.render(scene, 'test_conics.png', 300, 300,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {l1, l2})
