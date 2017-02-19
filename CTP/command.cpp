#define _CRT_SECURE_NO_WARNINGS

#include "Common.h"
#include "command.h"

#include <conio.h>

COORD command::coord;
char command::buffer[1024] = { 0 };
char* command::pbuf = buffer;
bool command::can_input = true;
std::thread* command::Analyzer = nullptr;

void command::thread_work()
{
	while (true)
	{
		if (char c = _getch())
		{
			if (!can_input)
				continue;
			_Get_Output_Mutex
			StoreCursorPosition(&coord);
			SetCursorPosition(pbuf - buffer + 10, 49);
			switch (c)
			{
				case VK_RETURN:
					{
						can_input = false;
						int len = (int)(pbuf - buffer);
						for (int i = 0;i < len; i++)
						{
							putchar('\b');
						}
						for (int i = 0;i < len; i++)
						{
							putchar(' ');
						}
						for (int i = 0;i < len; i++)
						{
							putchar('\b');
						}
						*pbuf = 0;
						Analyzer = new std::thread(analyze_command);
					}
					break;
				case VK_BACK:
					{
						if (pbuf>buffer)
						{
							pbuf--;
							puts("\b \b");
						}
					}
					break;
				default:
					{
						*(pbuf++) = c;
						putchar(c);
					}
					break;
			}
			RestoreCursorPosition(&coord);
			_Release_Output_Mutex
		}
	}
}

/*
* ���
*   h                ����
*   cl               �����Ļ
*   b [num]          ��num��
*   s [num]          ��num��
*   cb [num]         ƽ��num��
*   cs [num]         ƽ��num��
*   q                ֹͣ�Զ�����
*   rs               �����£���ʼ�Զ�����
*   exit             �˳�����
*/

void command::analyze_command()
{
	while (!Analyzer);
	char cmd[1024];
	strcpy(cmd, buffer);
	pbuf = buffer;
	can_input = true;

	switch (cmd[0])
	{
		case 'h':
			//_Get_Output_Mutex
			lgr << Log::t << "[cm]b [num]          ��num��" << Log::endl
				<< Log::t << "[cm]s [num]          ��num��" << Log::endl
				<< Log::t << "[cm]cb [num]         ƽ��num��" << Log::endl
				<< Log::t << "[cm]cs [num]         ƽ��num��" << Log::endl
				<< Log::t << "[cm]q                ֹͣ�Զ�����" << Log::endl
				<< Log::t << "[cm]rs               �����£���ʼ�Զ�����" << Log::endl
				<< Log::t << "[cm]exit             �˳�����" << Log::endl;
			//_Release_Output_Mutex
			break;
		case 'b':
			break;
		case 's':
			break;
		case 'c':
			{
				switch (cmd[1])
				{
					case 'b':
						break;
					case 's':
						break;
					case 'l':
						system("cls");
						InitScreen();
						break;
					default:
						_Get_Output_Mutex
						lgr << Log::t << "[cm]Illegal Input!" << Log::endl;
						_Release_Output_Mutex
						break;
				}
			}
		case 'q':
			break;
		case 'r':
			break;
		case 'e':
			break;
		default:
			_Get_Output_Mutex
			lgr << Log::t << "[cm]Illegal Input!" << Log::endl;
			_Release_Output_Mutex
			break;
	}

	Analyzer = nullptr;
}