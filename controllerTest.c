#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>

#define InvertStickY -1 // multiplied

#define AxisMax INT16_MAX
double controllerState_axis[SDL_CONTROLLER_AXIS_MAX] = {0};
uint32_t controllerState_buttons = 0; // 1 bit per button
int buttonIsPressed(int button) {
	return controllerState_buttons & (1<<button);
}
void printState(void) {
	printf("%6.2f           %6.2f\n",
		controllerState_axis[SDL_CONTROLLER_AXIS_TRIGGERLEFT],
		controllerState_axis[SDL_CONTROLLER_AXIS_TRIGGERRIGHT]
	);
	printf("   %c                %c\n",
		buttonIsPressed(SDL_CONTROLLER_BUTTON_LEFTSHOULDER) ? '#' : '+',
		buttonIsPressed(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) ? '#' : '+'
	);
	printf("   %c      %c  %c      %c\n",
		buttonIsPressed(SDL_CONTROLLER_BUTTON_DPAD_UP) ? '#' : '+',
		buttonIsPressed(SDL_CONTROLLER_BUTTON_BACK) ? '#' : '+',
		buttonIsPressed(SDL_CONTROLLER_BUTTON_START) ? '#' : '+',
		buttonIsPressed(SDL_CONTROLLER_BUTTON_Y) ? '#' : '+'
	);
	printf(" %c   %c            %c   %c\n",
		buttonIsPressed(SDL_CONTROLLER_BUTTON_DPAD_LEFT) ? '#' : '+',
		buttonIsPressed(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) ? '#' : '+',
		buttonIsPressed(SDL_CONTROLLER_BUTTON_X) ? '#' : '+',
		buttonIsPressed(SDL_CONTROLLER_BUTTON_B) ? '#' : '+'
	);
	printf("   %c                %c\n",
		buttonIsPressed(SDL_CONTROLLER_BUTTON_DPAD_DOWN) ? '#' : '+',
		buttonIsPressed(SDL_CONTROLLER_BUTTON_A) ? '#' : '+'
	);
	printf("%6.2f  %c      %c %6.2f\n",
		controllerState_axis[SDL_CONTROLLER_AXIS_LEFTX],
		buttonIsPressed(SDL_CONTROLLER_BUTTON_LEFTSTICK) ? '#' : '+',
		buttonIsPressed(SDL_CONTROLLER_BUTTON_RIGHTSTICK) ? '#' : '+',
		controllerState_axis[SDL_CONTROLLER_AXIS_RIGHTX]
	);
	printf("%6.2f           %6.2f\n",
		controllerState_axis[SDL_CONTROLLER_AXIS_LEFTY],
		controllerState_axis[SDL_CONTROLLER_AXIS_RIGHTY]
	);
	printf("LS angle:%6.3f  RS angle:%6.3f\n",
		atan2(controllerState_axis[SDL_CONTROLLER_AXIS_LEFTX], controllerState_axis[SDL_CONTROLLER_AXIS_LEFTY])/M_PI,
		atan2(controllerState_axis[SDL_CONTROLLER_AXIS_RIGHTX], controllerState_axis[SDL_CONTROLLER_AXIS_RIGHTY])/M_PI
	);
	printf("LS mgntd:%6.3f  RS mgntd:%6.3f\n",
		hypot(controllerState_axis[SDL_CONTROLLER_AXIS_LEFTX], controllerState_axis[SDL_CONTROLLER_AXIS_LEFTY]),
		hypot(controllerState_axis[SDL_CONTROLLER_AXIS_RIGHTX], controllerState_axis[SDL_CONTROLLER_AXIS_RIGHTY])
	);
	puts("\n");
}

char *axisName(int axis) {
	switch (axis) {
		case SDL_CONTROLLER_AXIS_INVALID: return "invalid";
		case SDL_CONTROLLER_AXIS_LEFTX: return "leftX";
		case SDL_CONTROLLER_AXIS_LEFTY: return "leftY";
		case SDL_CONTROLLER_AXIS_RIGHTX: return "rightX";
		case SDL_CONTROLLER_AXIS_RIGHTY: return "rightY";
		case SDL_CONTROLLER_AXIS_TRIGGERLEFT: return "triggerLeft";
		case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: return "triggerRight";
	}
	return "unrecognized";
}
char *buttonName(int button) {
	switch (button) {
		case SDL_CONTROLLER_BUTTON_INVALID: return "invalid";
		case SDL_CONTROLLER_BUTTON_A: return "a";
		case SDL_CONTROLLER_BUTTON_B: return "b";
		case SDL_CONTROLLER_BUTTON_X: return "x";
		case SDL_CONTROLLER_BUTTON_Y: return "y";
		case SDL_CONTROLLER_BUTTON_BACK: return "back";
		case SDL_CONTROLLER_BUTTON_GUIDE: return "guide";
		case SDL_CONTROLLER_BUTTON_START: return "start";
		case SDL_CONTROLLER_BUTTON_LEFTSTICK: return "leftStick";
		case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return "rightStick";
		case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return "leftShoulder";
		case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return "rightShoulder";
		case SDL_CONTROLLER_BUTTON_DPAD_UP: return "dpad_up";
		case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return "dpad_down";
		case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return "dpad_left";
		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return "dpad_right";
	}
	return "unrecognized button";
}

int openControllers(void) {
	int const numJoysticks = SDL_NumJoysticks();
	if (!numJoysticks) {
		SDL_Log("no controllers found\n");
		return 0;
	}
	SDL_GameController *ctrl;
	for (int i = 0; i < numJoysticks; ++i) {
		if (SDL_IsGameController(i)) {
			char *mapping;
			SDL_Log(
				"Index \'%i\' is a compatible controller, named \'%s\'",
				i, SDL_GameControllerNameForIndex(i)
			);
			ctrl = SDL_GameControllerOpen(i);
			mapping = SDL_GameControllerMapping(ctrl);
			SDL_Log("Controller %i is mapped as \"%s\".", i, mapping);
			SDL_free(mapping);
		}
		else SDL_Log("Index \'%i\' is not a compatible controller.", i);
	}
	return numJoysticks;
}

bool running = true;
void handleEvents(void) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT: puts("SDL_QUIT"); running = false; break;
			case SDL_WINDOWEVENT: {
				switch (event.window.event) {
					case SDL_WINDOWEVENT_RESIZED: {
						printf("window resized to %i by %i\n",
							event.window.data1, event.window.data2
						);
						break;
					}
				}
				break;
			}
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE: puts("escaped"); running = false; break;
				}
				break;
			case SDL_CONTROLLERAXISMOTION:
				//printf("%s axis motion: %i\n", axisName(event.caxis.axis), event.caxis.value);
				controllerState_axis[event.caxis.axis] = (double)event.caxis.value/AxisMax;
				if (
					event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY ||
					event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY
				) {
					controllerState_axis[event.caxis.axis] *= InvertStickY;
				}
				break;
			case SDL_CONTROLLERBUTTONDOWN:
				//printf("%s pressed\n", buttonName(event.cbutton.button));
				controllerState_buttons |= 1<<event.cbutton.button;
				break;
			case SDL_CONTROLLERBUTTONUP:
				//printf("%s released\n", buttonName(event.cbutton.button));
				controllerState_buttons &= ~(1<<event.cbutton.button);
				break;
			case SDL_CONTROLLERDEVICEADDED:
				//printf("controller %i added\n", event.cdevice.which);
				openControllers();
				break;
			case SDL_CONTROLLERDEVICEREMOVED:
				//printf("controller %i removed\n", event.cdevice.which);
				break;
			case SDL_CONTROLLERDEVICEREMAPPED:
				//printf("controller %i remapped\n", event.cdevice.which);
				break;
		}
	}
}


int main(int argc, char const *argv[]) {
	SDL_Init(SDL_INIT_GAMECONTROLLER);
	if (!openControllers()) return 0;
	while (running) {
		handleEvents();
		printState();
		SDL_Delay(16);
	}
	return 0;
}
