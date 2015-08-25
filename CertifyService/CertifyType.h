#pragma once

class UID
{
public:
	char m_data[9];
	char m_str[17];
	UID(char *data);
};

UID::UID(char *data)
{
	strcpy_s(m_data,data);
	sprintf_s(m_str,"%02x%02x%02x%02x%02x%02x%02x%02x",data[0]&0xff,data[1]&0xff,data[2]&0xff,data[3]&0xff,data[4]&0xff,data[5]&0xff,data[6]&0xff,data[7]&0xff);
}