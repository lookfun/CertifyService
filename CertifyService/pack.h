#pragma once
#pragma pack (1)

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
