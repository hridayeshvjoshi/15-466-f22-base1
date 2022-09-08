#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	// Translate asset data into PPU 
	// Assumes that asset is 8x8 and will perfectly fit on tile 
	void translateAsset(std::vector < glm::u8vec4 > data, uint32_t tt_index,
		 uint32_t pt_index);

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//some weird background animation:
	float background_fade = 0.0f;

	//player position:
	glm::vec2 player_at = glm::vec2(0.0f);

	// Game specifc attributes
	int numLives = 3;

	struct Bar {
		int dir; // 0 = up, 1 = down
	} leftBar, rightBar; 

	// For sprite dir
	struct Dir {
		int x = 1;
		int y = 1;
		int side; // 0 = left, 1 = right
		bool render; 
	};

	std::array< Dir, 64 > spriteDirs; 

	//----- drawing handled by PPU466 -----

	PPU466 ppu;
};
