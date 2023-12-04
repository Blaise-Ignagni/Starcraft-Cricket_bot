#pragma once


class GameItem {

public: 
	GameItem();
	GameItem(int indx, int val);

	int get_index() { return index; }
	int get_value() { return value; }
	int get_priority() { return priority; }
	int get_output() { return output; }
	
	void set_index(int indx) { index = indx; }
	void set_value(int val) { value = val; }
	void set_priority(int pri) { priority = pri; }
	void set_output(int op) { output = op; }

private:
	int index;
	int value;
	int priority;
	int output;
}; 