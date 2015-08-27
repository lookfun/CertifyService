#pragma once

class UID
{
public:
	char m_data[9];
	char m_str[17];
	UID(char *data);
};

class CODE
{
public:
	char m_data[33];
	char m_str[65];
	CODE(char *data);
	CODE();
};

void newcode(char * code);
char random();
void charto16x(char * in,char * out);