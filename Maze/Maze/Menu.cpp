#include "Menu.h"


void Menu::Selection(TextRenderer &text, GLuint &width, GLuint &height) {
	text.RenderText("Funny Maze", 530.0f, height / 2 - 100.0f, 2.0f, glm::vec3(1.0, 0.0, 0.0));
	text.RenderText("Select number how to make maze or find path.",0.0f, height / 2 - 40.0f, 2.0f, glm::vec3(1.0, 0.0, 0.0));
	text.RenderText("Press ENTER to start", 530.0f, height / 2 + 20.0f, 2.0f, glm::vec3(1.0, 0.0, 0.0));
}

void Menu::Win(TextRenderer &text, GLuint &width, GLuint &height) {
	text.RenderText(
		"Maze's generation or path-finding is over!!!", 120.0f, height / 2, 2.0, glm::vec3(1.0, 0.0, 0.0)
	);
	text.RenderText(
		"Press ENTER to restart or ESC to quit", 120.0f, height / 2 + 60, 2.0, glm::vec3(1.0, 0.0, 0.0)
	);
}
