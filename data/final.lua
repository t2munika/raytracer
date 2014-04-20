-- final scene
require('fish')

blue = gr.material({0.4, 0.6, 1}, {0.0, 0.0, 0.0}, 25, 0.96, 1.6666)
water = gr.material({100/255.0, 138/255.0, 227/255.0}, {0.0, 0.0, 0.0}, 25)
surface = gr.material({100/255.0, 138/255.0, 227/255.0}, {0.6, 0.6, 0.6}, 0)

ground = gr.material({161/255.0, 117/255.0, 24/255.0}, {0, 0, 0}, 25)
ground:set_normal_map("textures/sand.png")

stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)
stone:set_bump_map("textures/bumpy.png")

red_scales = gr.texture("textures/red_scales.png")
blue_scales = gr.texture("textures/blue_scales.png")
wood = gr.texture("textures/wood.png")
seashell = gr.texture("textures/seashell.png")

scene = gr.node('root')
room = gr.node('room')

-- the floor
floor = gr.plane('floor', 'xz')
floor:set_material(ground)
floor:translate(0, -7, -20)
floor:scale(10, 10, 10)
floor:rotate('X', 10)

left_wall = gr.plane('left', 'yz')
left_wall:set_material(water)
left_wall:translate(-7, 0, -20)
left_wall:scale(10, 10, 10)

-- right wall
right_wall = gr.mesh('right', {
		   { 0, -1, -1 }, 
		   { 0,  1, -1 }, 
		   { 0,  1,  1 }, 
		   { 0, -1,  1 }
		}, {
		   {3, 2, 1, 0}
		})
right_wall:set_material(water)
right_wall:translate(7, 0, -20)
right_wall:scale(10, 10, 10)
right_wall:rotate('x', 90)

-- ceiling
ceiling = gr.mesh('ceiling', {
		   { -1, 0, -1 }, 
		   {  1, 0, -1 }, 
		   {  1, 0,  1 }, 
		   { -1, 0,  1 }
		}, {
		   {0, 1, 2, 3}
		})
ceiling:set_material(surface)
ceiling:translate(0, 7, -20)
ceiling:scale(10, 10, 10)
ceiling:rotate('X', -10)

back_wall = gr.plane('back', 'xy')
back_wall:set_material(water)
back_wall:translate(0, 0, -25)
back_wall:scale(10, 10, 10)

room:add_child(back_wall)
room:add_child(right_wall)
room:add_child(left_wall)
room:add_child(ceiling)
room:add_child(floor)

ctop = gr.sphere('top')
ctop:translate(0, 0, -20)

b1 = gr.sphere('bubble')
b1:translate(-1, 1, -20)
b1:scale(0.5, 0.5, 0.5)
b1:set_material(blue)
scene:add_child(b1)

b2 = gr.sphere('bubble')
b2:translate(0, 0, -20)
b2:scale(0.5, 0.5, 0.5)
b2:set_material(blue)
scene:add_child(b2)

b3 = gr.sphere('bubble')
b3:translate(-0.8, -1.4, -12)
b3:scale(0.125, 0.125, 0.125)
b3:set_material(blue)
scene:add_child(b3)

b4 = gr.sphere('bubble')
b4:translate(-0.6, -1.7, -12)
b4:scale(0.125, 0.125, 0.125)
b4:set_material(blue)
scene:add_child(b4)

b5 = gr.sphere('bubble')
b5:translate(-0.8, -2, -12)
b5:scale(0.125, 0.125, 0.125)
b5:set_material(blue)
scene:add_child(b5)

r = gr.sphere('rock')
r:translate(2, -8, -20)
r:scale(2, 2, 2)
r:rotate('x', 10)
r:rotate('z', 45)
r:set_material(stone)
scene:add_child(r)

r2 = gr.sphere('rock')
r2:translate(3, -8, -19)
r2:scale(2, 2, 2)
r2:rotate('x', 10)
r2:rotate('z', 45)
r2:set_material(stone)
scene:add_child(r2)

r3 = gr.sphere('rock')
r3:translate(4, -8, -20)
r3:scale(2, 2, 2)
r3:rotate('x', 10)
r3:rotate('z', 45)
r3:set_material(stone)
scene:add_child(r3)


-- treasure chest
box = gr.cube('treasure chest')
box:translate(-3, -6, -22)
box:scale(3, 2, 2)
box:rotate('y', 20)
box:set_material(wood)
scene:add_child(box)

box_top = gr.cylinder('treasure chest top')
box_top:translate(-3, -5, -22)
box_top:scale(1.5, 1, 1)
box_top:rotate('y', 20)
box_top:rotate('z', 90)
box_top:set_material(wood)
scene:add_child(box_top)

-- fishes
fish1 = make_fish(blue_scales)
fish1:translate(0, 0, -23)
scene:add_child(fish1)

fish2 = make_fish(red_scales)
fish2:translate(-2.5, 2.5, -20)
fish2:rotate('z', -45)
scene:add_child(fish2)

fish3 = make_fish(blue_scales)
fish3:translate(-4, 4, -20)
fish3:rotate('z', 30)
scene:add_child(fish3)

fish4 = make_fish(red_scales)
fish4:translate(4, -2, -18)
fish4:rotate('y', 180)
scene:add_child(fish4)

fish5 = make_fish(blue_scales)
fish5:translate(-2, -2, -12)
fish5:rotate('y', 180)
scene:add_child(fish5)

fish6 = make_fish(blue_scales)
fish6:translate(3, 2, -15)
fish6:rotate('y', 180)
fish6:rotate('z', -20)
scene:add_child(fish6)

-- clam
clam = gr.intersect('clam')
clam:translate(-2, -4, -11)
clam:scale(0.6, 0.6, 0.6)

clam_s = gr.sphere('cs')
clam_s:set_material(seashell)
clam:add_child(clam_s)

clam_b = gr.sphere('cb')
clam_b:translate(0, -1, 0)
clam_b:set_material(seashell)
clam:add_child(clam_b)

scene:add_child(clam)
scene:add_child(room)

l1 = gr.light({0, 0, 0}, {0.4, 0.4, 0.4}, {1, 0, 0})
l2 = gr.light({0, 6.5, -20}, {0.7, 0.7, 0.7}, {1, 0, 0})
l3 = gr.light({-3, -5, -15}, {0.7, 0.7, 0.7}, {1, 0, 0})

gr.render(scene, 'final02.png', 500, 500,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {l1, l2}, 12, 0.3)
