#include "PlayMode.hpp"


//for the GL_ERRORS() macro:
#include "gl_errors.hpp"
#include "load_save_png.hpp"
#include "PPU466.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/epsilon.hpp>
#include <random>



void PlayMode::translateAsset(std::vector< glm::u8vec4 > data, uint32_t tt_index, 
	uint32_t pt_index) {
	assert(tt_index < ppu.tile_table.size());
	assert(pt_index < ppu.palette_table.size());
	
	// Retreive colors
	std::vector< glm::u8vec4 > colors;
	colors.push_back(data[0]);
	for (auto c : data) {
		if (colors.size() == 4) break;
		if (!glm::all(glm::equal(colors.back(), c))) {
			colors.push_back(c);
		}	
	} 
	auto c0 = colors[0]; auto c1 = colors[1];
	auto c2 = colors[2]; auto c3 = colors[3];

	// Define pallette 
	ppu.palette_table[pt_index] = { c0, c1, c2, c3 };

	// Set bits of tile 
	PPU466::Tile tile;
	std::array< uint8_t, 8> bit0 = {0, 0, 0, 0, 0, 0, 0, 0};
	std::array< uint8_t, 8> bit1 = {0, 0, 0, 0, 0, 0, 0, 0};
	for (uint32_t y = 0; y < 8; y++) {
		for (uint32_t x = 0; x < 8; x++) {
			glm::u8vec4 pixel = data[ 8 * y + x];

			if (glm::all(glm::equal(pixel, c0))) {
				continue;
			} else if (glm::all(glm::equal(pixel, c1))) {
				bit0[y] = bit0[y] | (1 << x);
			} else if (glm::all(glm::equal(pixel, c2))) {
				bit1[y] = bit1[y] | (1 << x);
			} else {
				bit0[y] = bit0[y] | (1 << x);
				bit1[y] = bit1[y] | (1 << x);
			}
		}
	}
	tile.bit0 = bit0;
	tile.bit1 = bit1;
	ppu.tile_table[tt_index] = tile;
}

PlayMode::PlayMode() {
	//TODO:
	// you *must* use an asset pipeline of some sort to generate tiles.
	// don't hardcode them like this!
	// or, at least, if you do hardcode them like this,
	//  make yourself a script that spits out the code that you paste in here
	//   and check that script into your repository.

	// Load sprites
	{
		glm::uvec2 tS = glm::uvec2(0x00, 0x00);
		glm::uvec2 *trialSize = &tS;
		std::vector< glm::u8vec4 > tD;
		std::vector< glm::u8vec4 > *trialData = &tD; 
		load_png("leftbar.png", trialSize, trialData, LowerLeftOrigin);
		this->translateAsset(*trialData, 30, 6);

		glm::uvec2 tS1 = glm::uvec2(0x00, 0x00);
		glm::uvec2 *trialSize1 = &tS1;
		std::vector< glm::u8vec4 > tD1;
		std::vector< glm::u8vec4 > *trialData1 = &tD1; 
		load_png("rightbar.png", trialSize1, trialData1, LowerLeftOrigin);
		this->translateAsset(*trialData1, 31, 1);
	}
	
	// Initialize locations of bars
	{
		ppu.sprites[0].x = 0;
		ppu.sprites[0].y = 0;
		ppu.sprites[1].x = 0;
		ppu.sprites[1].y = 8;
		ppu.sprites[2].x = 0;
		ppu.sprites[2].y = 16;
		ppu.sprites[3].x = 0;
		ppu.sprites[3].y = 24;
		ppu.sprites[4].x = 0;
		ppu.sprites[4].y = 32;

		ppu.sprites[5].x = 248;
		ppu.sprites[5].y = 0;
		ppu.sprites[6].x = 248;
		ppu.sprites[6].y = 8;
		ppu.sprites[7].x = 248;
		ppu.sprites[7].y = 16;
		ppu.sprites[8].x = 248;
		ppu.sprites[8].y = 24;
		ppu.sprites[9].x = 248;
		ppu.sprites[9].y = 32;
	}

	// Initialize 1 sprite

	//Also, *don't* use these tiles in your game:

	// { //use tiles 0-16 as some weird dot pattern thing:
	// 	std::array< uint8_t, 8*8 > distance;
	// 	for (uint32_t y = 0; y < 8; ++y) {
	// 		for (uint32_t x = 0; x < 8; ++x) {
	// 			float d = glm::length(glm::vec2((x + 0.5f) - 4.0f, (y + 0.5f) - 4.0f));
	// 			d /= glm::length(glm::vec2(4.0f, 4.0f));
	// 			distance[x+8*y] = uint8_t(std::max(0,std::min(255,int32_t( 255.0f * d ))));
	// 		}
	// 	}
	// 	for (uint32_t index = 0; index < 16; ++index) {
	// 		PPU466::Tile tile;
	// 		uint8_t t = uint8_t((255 * index) / 16);
	// 		for (uint32_t y = 0; y < 8; ++y) {
	// 			uint8_t bit0 = 0;
	// 			uint8_t bit1 = 0;
	// 			for (uint32_t x = 0; x < 8; ++x) {
	// 				uint8_t d = distance[x+8*y];
	// 				if (d > t) {
	// 					bit0 |= (1 << x);
	// 				} else {
	// 					bit1 |= (1 << x);
	// 				}
	// 			}
	// 			tile.bit0[y] = bit0;
	// 			tile.bit1[y] = bit1;
	// 		}
	// 		ppu.tile_table[index] = tile;
	// 	}
	// }

	// //use sprite 32 as a "player":
	// ppu.tile_table[32].bit0 = {
	// 	0b01111110,
	// 	0b11111111,
	// 	0b11111111,
	// 	0b11111111,
	// 	0b11111111,
	// 	0b11111111,
	// 	0b11111111,
	// 	0b01111110,
	// };
	// ppu.tile_table[32].bit1 = {
	// 	0b00000000,
	// 	0b00000000,
	// 	0b00011000,
	// 	0b00100100,
	// 	0b00000000,
	// 	0b00100100,
	// 	0b00000000,
	// 	0b00000000,
	// };

	// //makes the outside of tiles 0-16 solid:
	// ppu.palette_table[0] = {
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	// };

	// //makes the center of tiles 0-16 solid:
	// ppu.palette_table[1] = {
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	// };

	// // used for the player:
	// ppu.palette_table[7] = {
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	// 	glm::u8vec4(0xff, 0xff, 0x00, 0xff),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	// };

	// //used for the misc other sprites:
	// ppu.palette_table[6] = {
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	// 	glm::u8vec4(0x88, 0x88, 0xff, 0xff),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	// 	glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	// };

}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} 
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.pressed = false;
			return true;
		}
	}

	return false;


	
	// if (evt.type == SDL_KEYDOWN) {
	// 	if (evt.key.keysym.sym == SDLK_LEFT) {
	// 		left.downs += 1;
	// 		left.pressed = true;
	// 		return true;
	// 	} else if (evt.key.keysym.sym == SDLK_RIGHT) {
	// 		right.downs += 1;
	// 		right.pressed = true;
	// 		return true;
	// 	} else if (evt.key.keysym.sym == SDLK_UP) {
	// 		up.downs += 1;
	// 		up.pressed = true;
	// 		return true;
	// 	} else if (evt.key.keysym.sym == SDLK_DOWN) {
	// 		down.downs += 1;
	// 		down.pressed = true;
	// 		return true;
	// 	}
	// } else if (evt.type == SDL_KEYUP) {
	// 	if (evt.key.keysym.sym == SDLK_LEFT) {
	// 		left.pressed = false;
	// 		return true;
	// 	} else if (evt.key.keysym.sym == SDLK_RIGHT) {
	// 		right.pressed = false;
	// 		return true;
	// 	} else if (evt.key.keysym.sym == SDLK_UP) {
	// 		up.pressed = false;
	// 		return true;
	// 	} else if (evt.key.keysym.sym == SDLK_DOWN) {
	// 		down.pressed = false;
	// 		return true;
	// 	}
	// }

	// return false;
}

// Sprites 1-5 will be used for left bar
// Sprites 6-10 will be used for right bar 

void PlayMode::update(float elapsed) {

	constexpr uint8_t RegularSpeed = 70;
	constexpr uint8_t HighSpeed = 150;

	if (left.pressed) {
		if (leftBar.dir == 0) { // going up
			ppu.sprites[0].y += (uint8_t) (HighSpeed * elapsed);
			ppu.sprites[1].y += (uint8_t) (HighSpeed * elapsed);
			ppu.sprites[2].y += (uint8_t) (HighSpeed * elapsed);
			ppu.sprites[3].y += (uint8_t) (HighSpeed * elapsed);
			ppu.sprites[4].y += (uint8_t) (HighSpeed * elapsed);
		} else {  				// going down
			ppu.sprites[0].y -= (uint8_t) (HighSpeed * elapsed);
			ppu.sprites[1].y -= (uint8_t) (HighSpeed * elapsed);
			ppu.sprites[2].y -= (uint8_t) (HighSpeed * elapsed);
			ppu.sprites[3].y -= (uint8_t) (HighSpeed * elapsed);
			ppu.sprites[4].y -= (uint8_t) (HighSpeed * elapsed);
		}
	} else {
		if (leftBar.dir == 0) { // going up
			ppu.sprites[0].y += (uint8_t) (RegularSpeed * elapsed);
			ppu.sprites[1].y += (uint8_t) (RegularSpeed * elapsed);
			ppu.sprites[2].y += (uint8_t) (RegularSpeed * elapsed);
			ppu.sprites[3].y += (uint8_t) (RegularSpeed * elapsed);
			ppu.sprites[4].y += (uint8_t) (RegularSpeed * elapsed);
		} else {  				// going down
			ppu.sprites[0].y -= (uint8_t) (RegularSpeed * elapsed);
			ppu.sprites[1].y -= (uint8_t) (RegularSpeed * elapsed);
			ppu.sprites[2].y -= (uint8_t) (RegularSpeed * elapsed);
			ppu.sprites[3].y -= (uint8_t) (RegularSpeed * elapsed);
			ppu.sprites[4].y -= (uint8_t) (RegularSpeed * elapsed);
		}
	}

	if (right.pressed) {
		if (rightBar.dir == 0) { // going up
			ppu.sprites[5].y += (uint8_t) (HighSpeed * elapsed);
			ppu.sprites[6].y += (uint8_t) (HighSpeed * elapsed);
			ppu.sprites[7].y += (uint8_t) (HighSpeed * elapsed);
			ppu.sprites[8].y += (uint8_t) (HighSpeed * elapsed);
			ppu.sprites[9].y += (uint8_t) (HighSpeed * elapsed);
		} else {  				// going down
			ppu.sprites[5].y -= (uint8_t) (HighSpeed * elapsed);
			ppu.sprites[6].y -= (uint8_t) (HighSpeed * elapsed);
			ppu.sprites[7].y -= (uint8_t) (HighSpeed * elapsed);
			ppu.sprites[8].y -= (uint8_t) (HighSpeed * elapsed);
			ppu.sprites[9].y -= (uint8_t) (HighSpeed * elapsed);
		}
	} else {
		if (rightBar.dir == 0) { // going up
			ppu.sprites[5].y += (uint8_t) (RegularSpeed * elapsed);
			ppu.sprites[6].y += (uint8_t) (RegularSpeed * elapsed);
			ppu.sprites[7].y += (uint8_t) (RegularSpeed * elapsed);
			ppu.sprites[8].y += (uint8_t) (RegularSpeed * elapsed);
			ppu.sprites[9].y += (uint8_t) (RegularSpeed * elapsed);
		} else {  				// going down
			ppu.sprites[5].y -= (uint8_t) (RegularSpeed * elapsed);
			ppu.sprites[6].y -= (uint8_t) (RegularSpeed * elapsed);
			ppu.sprites[7].y -= (uint8_t) (RegularSpeed * elapsed);
			ppu.sprites[8].y -= (uint8_t) (RegularSpeed * elapsed);
			ppu.sprites[9].y -= (uint8_t) (RegularSpeed * elapsed);
		}
	}

	// Flip bar direction
	if (leftBar.dir == 0 && ppu.sprites[4].y >= ppu.ScreenHeight) {
		leftBar.dir = 1;
	} else if (leftBar.dir == 1 && ppu.sprites[0].y <= 0) {
		leftBar.dir = 0;
	}
	if (rightBar.dir == 0 && ppu.sprites[9].y >= ppu.ScreenHeight) {
		rightBar.dir = 1;
	}
	//reset button press counters:
	left.downs = 0;
	right.downs = 0;

	// //slowly rotates through [0,1):
	// // (will be used to set background color)
	// background_fade += elapsed / 10.0f;
	// background_fade -= std::floor(background_fade);

	// constexpr float PlayerSpeed = 30.0f;
	// if (left.pressed) player_at.x -= PlayerSpeed * elapsed;
	// if (right.pressed) player_at.x += PlayerSpeed * elapsed;
	// if (down.pressed) player_at.y -= PlayerSpeed * elapsed;
	// if (up.pressed) player_at.y += PlayerSpeed * elapsed;

	// //reset button press counters:
	// left.downs = 0;
	// right.downs = 0;
	// up.downs = 0;
	// down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	//background color will be some hsv-like fade:
	ppu.background_color = glm::u8vec4(
		0x00,
		0x00,
		0x00,
		0xff
	);

	// Bar Sprites
	for (uint32_t i = 0; i < 5; i++) {
		ppu.sprites[0].index = 30;
		ppu.sprites[0].attributes = 6;
	}

	for (uint32_t i = 5; i < 10; i++) {
		ppu.sprites[0].index = 31;
		ppu.sprites[0].attributes = 7;
	}


	// Box Sprites
	for (uint32_t i = 10; i < 64; i++) {
		// Set index/attributes
		if(spriteDirs[i].side == 0) { //going left
			ppu.sprites[0].index = 30;
			ppu.sprites[0].attributes = 6;
		} else {
			ppu.sprites[0].index = 31;
			ppu.sprites[0].attributes = 7;
		}
		
		// Update positions 

		// Account for bounces and lives lost
		if (spriteDirs[i].render) {
			if (spriteDirs[i].side == 0 && ppu.sprites[i].x <= 0
				|| spriteDirs[i].side == 1 && ppu.sprites[i].x >= ppu.ScreenWidth) {
				numLives--;
				spriteDirs[i].render = false;
			}

			if (ppu.sprites[i].y <= 0 || ppu.sprites[i].y >= ppu.ScreenHeight) {
				spriteDirs[i].y *= -1;
			}
		}
	}

	//--- actually draw ---
	ppu.draw(drawable_size);


	// //background color will be some hsv-like fade:
	// ppu.background_color = glm::u8vec4(
	// 	std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 0.0f / 3.0f) ) ) ))),
	// 	std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 1.0f / 3.0f) ) ) ))),
	// 	std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 2.0f / 3.0f) ) ) ))),
	// 	0xff
	// );

	// //tilemap gets recomputed every frame as some weird plasma thing:
	// //NOTE: don't do this in your game! actually make a map or something :-)
	// for (uint32_t y = 0; y < PPU466::BackgroundHeight; ++y) {
	// 	for (uint32_t x = 0; x < PPU466::BackgroundWidth; ++x) {
	// 		//TODO: make weird plasma thing
	// 		ppu.background[x+PPU466::BackgroundWidth*y] = ((x+y)%16);
	// 	}
	// }

	// //background scroll:
	// ppu.background_position.x = int32_t(-0.5f * player_at.x);
	// ppu.background_position.y = int32_t(-0.5f * player_at.y);

	// //player sprite:
	// ppu.sprites[0].x = int8_t(	player_at.x);
	// ppu.sprites[0].y = int8_t(player_at.y);
	// ppu.sprites[0].index = 32;
	// ppu.sprites[0].attributes = 7;

	// //some other misc sprites:
	// for (uint32_t i = 1; i < 63; ++i) {
	// 	float amt = (i + 2.0f * background_fade) / 62.0f;
	// 	ppu.sprites[i].x = int8_t(0.5f * PPU466::ScreenWidth + std::cos( 2.0f * M_PI * amt * 5.0f + 0.01f * player_at.x) * 0.4f * PPU466::ScreenWidth);
	// 	ppu.sprites[i].y = int8_t(0.5f * PPU466::ScreenHeight + std::sin( 2.0f * M_PI * amt * 3.0f + 0.01f * player_at.y) * 0.4f * PPU466::ScreenWidth);
	// 	ppu.sprites[i].index = 32;
	// 	ppu.sprites[i].attributes = 6;
	// 	if (i % 2) ppu.sprites[i].attributes |= 0x80; //'behind' bit
	// }

	// //--- actually draw ---
	// ppu.draw(drawable_size);
}
