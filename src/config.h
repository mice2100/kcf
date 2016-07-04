#pragma once

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include <map>

using namespace std;

class config{
public:
	config(){};
	bool 	Init();
	int		GetConfigInt(string name);
	float	GetConfigFlt(string name);
	string	GetConfigStr(string name);

private:
	map<string, string> m_configmap;
};

#endif
