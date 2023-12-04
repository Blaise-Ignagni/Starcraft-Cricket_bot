

#include "GameItem.h"

GameItem::GameItem() {
	index = 0;
	value = 0;
	priority = 0;
	output = 0;
}

GameItem::GameItem(int indx, int val) {
	index = indx;
	value = val;
	priority = 0;
	output = 0;
}