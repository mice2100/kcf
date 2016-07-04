#include "config.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

bool config::Init()
{
	ifstream cf("config.txt");
	if(!cf.is_open())
		return false;
	string strLine;
	while(!cf.eof())
	{
		cf>>strLine;
		int pos = strLine.find("=");
		m_configmap.insert(make_pair(strLine.substr(0, pos), strLine.substr(pos+1)));
	}
}

int	config::GetConfigInt(string name)
{
	string val = m_configmap[name];
	
	if(val.length()<=0)
		return 0;
		
	stringstream ss(val);
	int ret;
	ss>>ret;
	
	return ret;
	
}

float config::GetConfigFlt(string name)
{
	string val = m_configmap[name];
	
	if(val.length()<=0)
		return 0.0;
		
	stringstream ss(val);
	float ret;
	ss>>ret;
	
	return ret;
	
}

string config::GetConfigStr(string name)
{
	return m_configmap[name];
}
