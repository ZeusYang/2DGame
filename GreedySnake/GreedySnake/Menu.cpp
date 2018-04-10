#include "Menu.h"


void Menu::Selection(TextRenderer &text, GLuint &width, GLuint &height) {
	text.RenderText("Greedy Snake", 230.0f, height / 2 - 100.0f, 2.0f, glm::vec3(1.0, 0.6, 0.3));
	text.RenderText("Enjoy the fighting of AI", 230.0f, height / 2 - 40.0f, 1.0f);
	//text.RenderText("1 -> Play by hands.", 230.0f, height / 2 - 20.0f, 1.0f);
	//text.RenderText("2 -> Play by AI.", 230.0f, height / 2, 1.0f);
	text.RenderText("Press ENTER to start", 230.0f, height / 2 - 20.0f, 1.0f);
}

void Menu::Win(TextRenderer &text, GLuint &width, GLuint &height) {
	text.RenderText(
		"You WON!!!", 310.0f, height / 2 - 20.0, 1.0, glm::vec3(1.0, 1.0, 0.0)
	);
	text.RenderText(
		"Press ENTER to retry or ESC to quit", 150.0f, height / 2, 1.0, glm::vec3(1.0, 1.0, 0.0)
	);
}

void Menu::Lost(TextRenderer &text, GLuint &width, GLuint &height, std::string &score) {

	text.RenderText(
		"AI got " + score + " points!", 310.0, height / 2 - 20.0, 1.0, glm::vec3(1.0, 1.0, 0.0)
	);
	text.RenderText(
		"Press ENTER to retry or ESC to quit", 150.0, height / 2, 1.0, glm::vec3(1.0, 1.0, 0.0)
	);
}