#include "pch.h"

static const int PANEL_HEIGHT = 7;
static const int BAR_WIDTH = 20;
static const int MSG_X = BAR_WIDTH + 2;
static const int MSG_HEIGHT = PANEL_HEIGHT - 1;
const int PAUSE_MENU_WIDTH = 30;
const int PAUSE_MENU_HEIGHT = 15;


Gui::Gui()
{
	cons = new TCODConsole(engine.get_screen_width(), PANEL_HEIGHT);
}


Gui::~Gui()
{
	delete cons;
	clear();
}

void Gui::render()
{
	//clear GUI
	cons->setDefaultBackground(TCODColor::black);
	cons->clear();

	//draw hp bar
	renderBar(1, 1, BAR_WIDTH, "HP", engine.player->destructible->get_hp(),
		engine.player->destructible->get_maxHp(), TCODColor::lightRed, TCODColor::darkerRed);

	//draw xp bar
	PlayerAi *ai = (PlayerAi*)engine.player->ai;
	char xpTxt[128];
	sprintf(xpTxt, "XP(%d)", ai->get_xp_level());
	renderBar(1, 5, BAR_WIDTH, xpTxt, engine.player->destructible->get_xp(),
		ai->getNextLevelXp(), TCODColor::lightViolet, TCODColor::darkerViolet);

	//dungeon lvl
	cons->setDefaultForeground(TCODColor::white);
	cons->printf(3, 3, "Dungeon level %d", engine.get_level());

	//draw message log
	int y = 1;
	float colorCoef = 0.4f;
	for (Message **it = log.begin(); it != log.end(); it++) {
		Message *message = *it;
		cons->setDefaultForeground(message->col * colorCoef);
		std::string buf = std::string(message->get_text());
		cons->printf(MSG_X, y, message->get_text());
		y++;
		if (colorCoef < 1.0f) {
			colorCoef += 0.3f;
		}
	}

	// mouse look
	renderMouseLook();

	//set gui on root console
	TCODConsole::blit(cons, 0, 0, engine.get_screen_width(), PANEL_HEIGHT,
		TCODConsole::root, 0, engine.get_screen_height() - PANEL_HEIGHT);

	
}

void Gui::renderMouseLook()
{
	if (!engine.map->isInFov(engine.get_mouse().cx, engine.get_mouse().cy)) {
		//mouse out of fov
		return;
	}

	char buff[128] = "";
	bool first = true;

	for (Actor **it = engine.actors.begin(); it != engine.actors.end(); it++) {
		Actor *actor = *it;
		//find actors under mouse cursor
		if (actor->get_x_pos() == engine.get_mouse().cx &&
			actor->get_y_pos() == engine.get_mouse().cy) {
			if (!first) {
				strcat(buff, ", ");
			}
			else {
				first = false;
			}
			strcat(buff, actor->get_name());
		}
	}
	// display the list of actors under the mouse cursor
	cons->setDefaultForeground(TCODColor::lightGrey);
	cons->printf(1, 0, buff);
}

void Gui::renderBar(int x, int y, int width, const char * name, float value, float maxValue, const TCODColor & barColor, const TCODColor & backColor)
{
	//fill background
	cons->setDefaultBackground(backColor);
	cons->rect(x, y, width, 1, false, TCOD_BKGND_SET);

	int barWidth = (int)(value / maxValue * width);
	if (barWidth > 0) {
		//draw
		cons->setDefaultBackground(barColor);
		cons->rect(x, y, barWidth, 1, false, TCOD_BKGND_SET);
		//text
		cons->setDefaultForeground(TCODColor::white);
		cons->printf(x + width / 2, y, TCOD_BKGND_NONE, TCOD_CENTER,
			"%s : %g/%g", name, value, maxValue);
	}
}

void Gui::message(const TCODColor & col, const char * text, ...)
{
	va_list ap; //list of unknown param
	char buff[128];
	__crt_va_start(ap, text);//init
	vsprintf(buff, text, ap);//print our message
	__crt_va_end(ap);//clean up

	char *lineBegin = buff;
	char *lineEnd;

	do {
		//make room for new message
		if (log.size() == MSG_HEIGHT) {
			Message *toRemove = log.get(0);
			log.remove(toRemove);
			delete toRemove;
		}

		lineEnd = strchr(lineBegin, '\n');
		if (lineEnd) {
			*lineEnd = '\0';
		}

		//add new message
		Message *msg = new Message(lineBegin, col);
		log.push(msg);
		//next line
		lineBegin = lineEnd + 1;
	} while (lineEnd);
}

void Gui::load(TCODZip & zip)
{
	int nbMessages = zip.getInt();
	while (nbMessages > 0) {
		const char *text = zip.getString();
		TCODColor col = zip.getColor();
		message(col, text);
		nbMessages--;
	}
}

void Gui::save(TCODZip & zip)
{
	zip.putInt(log.size());
	for (Message **it = log.begin(); it != log.end(); it++) {
		zip.putString((*it)->text);
		zip.putColor(&(*it)->col);
	}
}

void Gui::clear()
{
	log.clearAndDelete();
}

Gui::Message::Message(const char * text, const TCODColor & col) : 
	text(_strdup(text)), col(col)
{
}

Gui::Message::~Message()
{
	free(text);
}

Menu::~Menu()
{
	clear();
}

void Menu::clear()
{
	items.clearAndDelete();
}

void Menu::addItem(MenuItemCode code, const char * label)
{
	MenuItem *item = new MenuItem();
	item->code = code;
	item->label = label;
	items.push(item);
}

Menu::MenuItemCode Menu::pick(DisplayMode mode)
{
	int selectedItem = 0;
	int menux, menuy;

	if (mode == PAUSE) {
		menux = engine.get_screen_width() / 2 - PAUSE_MENU_WIDTH / 2;
		menuy = engine.get_screen_height() / 2 - PAUSE_MENU_HEIGHT / 2;
		TCODConsole::root->setDefaultForeground(TCODColor(200, 180, 50));
		TCODConsole::root->printFrame(menux, menuy, PAUSE_MENU_WIDTH, PAUSE_MENU_HEIGHT, true,
			TCOD_BKGND_ALPHA(70), "menu");
		menux += 2;
		menuy += 2;
	}
	else {
		static TCODImage img("grame.png");
		img.blit2x(TCODConsole::root, 25, 20);
		menux = 20;
		menuy = TCODConsole::root->getHeight() / 3;
	}
	
	while (!TCODConsole::isWindowClosed()) {
		int currentItem = 0;
		for (MenuItem **it = items.begin(); it != items.end(); it++) {
			if (currentItem == selectedItem) {
				TCODConsole::root->setDefaultForeground(TCODColor::lightOrange);
			}
			else {
				TCODConsole::root->setDefaultForeground(TCODColor::lightGrey);
			}
			TCODConsole::root->printf(menux, menuy + currentItem * 5, (*it)->label);
			currentItem++;
		}
		TCODConsole::flush();
		//check key presses
		TCOD_key_t key;
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, nullptr);
		switch (key.vk) {
		case TCODK_UP:
			selectedItem--;
			if (selectedItem < 0) {
				selectedItem = items.size() - 1;
			}
			break;
		case TCODK_DOWN:
			selectedItem = (selectedItem + 1) % items.size();
			break;
		case TCODK_ENTER:
			return items.get(selectedItem)->code;
		default:break;
		}
	}
	return NONE;
}
