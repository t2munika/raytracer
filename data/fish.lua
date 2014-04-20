
-- crude fish
function make_fish(mat) 
	local fish = gr.node('fish')
	local black = gr.material({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 0)

	local eye = gr.sphere('eye')
	eye:translate(-0.75, 0, 0.3)
	eye:scale(0.05, 0.05, 0.05)
	eye:set_material(black)
	fish:add_child(eye)

	local eye2 = gr.sphere('eye')
	eye2:translate(-0.75, 0, -0.3)
	eye2:scale(0.05, 0.05, 0.05)
	eye2:set_material(black)
	fish:add_child(eye2)

	local fish_body = gr.sphere('fish body')
	fish_body:translate(0, 0, 0)
	fish_body:scale(1, 0.5, 0.5)
	fish_body:set_material(mat)
	fish:add_child(fish_body)

	local fish_tail = gr.cone('fish tail')
	fish_tail:translate(1.25, 0, 0)
	fish_tail:rotate('z', 90)
	fish_tail:scale(0.5, 1, 0.5)
	fish_tail:set_material(mat)
	fish:add_child(fish_tail)

	local fish_dorsal = gr.sphere('fish dorsal fin')
	fish_dorsal:translate(0, 0.4, 0)
	fish_dorsal:scale(0.5, 0.25, 0.25)
	fish_dorsal:set_material(mat)
	fish:add_child(fish_dorsal)

	local fish_fin1 = gr.sphere('fish dorsal fin')
	fish_fin1:translate(-0.1, -0.4, 0)
	fish_fin1:scale(0.15, 0.25, 0.15)
	fish_fin1:set_material(mat)
	fish:add_child(fish_fin1)

	local fish_fin2 = gr.sphere('fish dorsal fin')
	fish_fin2:translate(0.1, -0.4, 0)
	fish_fin2:scale(0.15, 0.25, 0.15)
	fish_fin2:set_material(mat)
	fish:add_child(fish_fin2)

	return fish
end
