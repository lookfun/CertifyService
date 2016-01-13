#pragma once
#pragma pack (1)

#define SEARCH	(0xcc)
#define INSERT	(0x33)
#define SET		(0xaa)
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

enum state
{
	VARIFY_SECCESS,
	VARIFY_FAILED,
	SET_SECCESS
};

class Frame
{
public:
	char uid[8];
	char code[32];
	Frame(){};
	Frame(state st, char* cd);

	int gen(state st, char* cd);
	int gen(char* uid, char* cd);
};
