-- makes a room.

wall = gr.material({244/255.0, 247/255.0, 225/255.0}, {0.0, 0.0, 0.0}, 0)

room = gr.node('room')

-- the floor
floor = gr.mesh('floor', {
		   { -1, 0, -1 }, 
		   {  1, 0, -1 }, 
		   {  1,  0, 1 }, 
		   { -1, 0, 1  }
		}, {
		   {3, 2, 1, 0}
		})
floor:set_material(wall)
floor:translate(0, -7, -20)
floor:scale(10, 10, 10)
floor:rotate('X', 10)

-- left wall
left_wall = gr.mesh('left', {
		   { 0, -1, -1 }, 
		   { 0,  1, -1 }, 
		   { 0,  1,  1 }, 
		   { 0, -1,  1 }
		}, {
		   {0, 1, 2, 3}
		})
left_wall:set_material(wall)
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
right_wall:set_material(wall)
right_wall:translate(7, 0, -20)
right_wall:scale(10, 10, 10)

-- ceiling
ceiling = gr.mesh('ceiling', {
		   { -1, 0, -1 }, 
		   {  1, 0, -1 }, 
		   {  1, 0,  1 }, 
		   { -1, 0,  1 }
		}, {
		   {0, 1, 2, 3}
		})
ceiling:set_material(wall)
ceiling:translate(0, 7, -20)
ceiling:scale(10, 10, 10)
ceiling:rotate('X', -10)

-- back wall
back_wall = gr.mesh('back', {
		   { -1, -1, 0 }, 
		   { -1,  1, 0 }, 
		   {  1,  1, 0 }, 
		   {  1, -1, 0 }
		}, {
		   {3, 2, 1, 0}
		})
back_wall:set_material(wall)
back_wall:translate(0, 0, -25)
back_wall:scale(10, 10, 10)

room:add_child(back_wall)
room:add_child(right_wall)
room:add_child(left_wall)
room:add_child(ceiling)
room:add_child(floor)
