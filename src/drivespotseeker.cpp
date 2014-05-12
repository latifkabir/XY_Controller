#include "spotseeker.h"
#include "math.h"

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

void runShell();
void displayHelp();

int main(int argc, char *argv[])
{
	displayHelp();	
	runShell();	
	return 0;
}

template <class T>
bool from_string(T& t, const std::string& s, std::ios_base& (*f)(std::ios_base&))
{
	std::istringstream iss(s);
	return !(iss >> f >> t).fail();
};

void runShell()
{
	SpotSeeker theBot;
	
	const int strLimit=200;
	char cmdtype;
	char cmdsubtype;
	int cmdvalue;

	int str_size;
	char str_char[strLimit+1];
	int i;
	int act_pulses;
	int Verbose;
	bool steady=false;

	//initially powered off, in non-verbose mode

	do {
		//Wait for a PC command ( CR terminated string)
		cout<<"COMMAND:";
		str_size=cin.getline(str_char,strLimit).gcount()-1;

		//'Mininum command is two characters and a <CR>, otherwise discard
		if ( str_size < 2  && toupper(str_char[0]) != 'H' && toupper(str_char[0]) !='Q')
		{
			cout<<"Error: <2 Char"<<endl;
			continue;
		}

		cmdtype = toupper(str_char[0]);      //get value of 0th character in command string
		cmdsubtype = toupper(str_char[1]);
		cmdvalue   = 0;             //zero by default

		//If more than just two-letters + <CR>, then cmdvalue present also

		if (str_size > 2)
		{
			if (!from_string<int>(cmdvalue, &str_char[2], std::dec))
				cmdtype=-1;
		}

		//Test 0th character of command string
		switch (cmdtype)
		{
		case 'Q':
			return;
			break;		
		case -1:
			//'Numeric error
			cout<<"Error: Invalid Number";
			break;

		case 'H':
			displayHelp();
			break;

		case 'V':
			//'Verbose reply to M commands
			if (cmdsubtype == '1')
			{
				Verbose = 1;
				cout<<"1";
			}
			else
			{
				Verbose = 0;
				cout<<"0";
			}
			break;

		case 'P':
			//'Power Control
			if (cmdsubtype == '1')         // 'P1
			{				
				if (Verbose)
					cout<<"Power: On";
				else
					cout<<"1";
			}
			else                          //'P0
			{
				if (Verbose)
					cout<<"Power: Off";
				else
					cout<<"0";
			}
			break;
		case 'S':
			//'Power Control
			if (cmdsubtype == '1')         // 'P1
			{				
				if (Verbose)
					cout<<"Steady: On";
				else
					cout<<"1";
				steady=true;
			}
			else                          //'P0
			{
				if (Verbose)
					cout<<"Steady: Off";
				else
					cout<<"0";
				steady=false;
			}
			break;


		case 'L':
			//'Location Set/Get
			switch (cmdsubtype)
			{
			case '0':
				//Set the current location as our origin (0,0,0,0), (x,y,z,r)
				theBot.setOrigin();
				cout<<"0";
				break;

			case 'X':
				cout<<theBot.getPosition().x;
				break;
			case 'Y':
				cout<<theBot.getPosition().y;
				break;
			case 'Z':
				cout<<theBot.getPosition().z;
				break;
			case 'R':
				cout<<theBot.getPosition().r;
				break;
			default:
				cout<<"Error: Invalid SubCmd";
				break;
			}
		case 'M':
			//'Move Command
			switch(cmdsubtype)
			{
			case 'X':
				//'Move X Stepper
				if (Verbose)
				{
					if (cmdvalue < 0)
						cout<<"Left: ";
					else
						cout<<"Right: ";
					cout<<cmdvalue;
					cout<<", Moved: ";
				}
				act_pulses = theBot.moveLink(0,cmdvalue,steady);
				cout<<act_pulses;
				break;
			case 'Y':
				//'Move Y Stepper
				if (Verbose)
				{
					if (cmdvalue < 0)
						cout<<"Down: ";
					else
						cout<<"Up: ";
					cout<<cmdvalue;
					cout<<", Moved: ";
				}
				act_pulses = theBot.moveLink(1,cmdvalue,steady);
				cout<<act_pulses;
				break;
			case 'Z':
				//'Move Z Stepper
				if (Verbose)
				{
					if (cmdvalue < 0)
						cout<<"Out: ";
					else
						cout<<"In: ";
					cout<<cmdvalue;
					cout<<", Moved: ";
				}
				act_pulses = theBot.moveLink(2,cmdvalue,steady);
				cout<<act_pulses;
				break;
			case 'R':
				//'Move R Stepper
				if (Verbose)
				{
					if (cmdvalue < 0)
						cout<<"CCW: ";
					else
						cout<<"CW: ";
					cout<<cmdvalue;
					cout<<", Moved: ";
				}
				act_pulses = theBot.moveLink(3,cmdvalue,steady);
				cout<<act_pulses;
				break;
			default:
				cout<<"Error: Invalid SubCmd";
				break;
			}
			break;
		default:
			cout<<"Error: Invalid Cmd";
			break;
		}
		cout<<endl;
	} while(true);
}

void displayHelp()
{
	cout<<"Power must be ON prior to move commands! Default: OFF"<<endl;
	cout<<"P1 : Power On"<<endl;
	cout<<"P0 : Power Off"<<endl<<endl;

	cout<<"Q : Quit"<<endl<<endl;
	cout<<"X & Y : 3200 steps/inch, or 0.3125 mils/step"<<endl;
	cout<<"    Z : 1000 steps/inch, or 1 mil/step"<<endl;
	cout<<"    R : 2400 steps/revolution, or 0.15deg/step"<<endl<<endl;

	cout<<"MX-3200 : Move X 3200 Pulses Left(-) [-1.0 inch]"<<endl;
	cout<<"MY1600  : Move Y 1600 Pulses Up(+)   [+0.5 inch]"<<endl;
	cout<<"MZ1000  : Move Z 1000 Pulses In(+)   [+1.0 inch]"<<endl;
	cout<<"MR600   : Move Z  600 Pulses CW(+)   [ +90 degs]"<<endl<<endl;
	
	cout<<"SX-3200 : Steadily Move X 3200 Pulses Left(-) [-1.0 inch]"<<endl;
	cout<<"SY1600  : Steadily Move Y 1600 Pulses Up(+)   [+0.5 inch]"<<endl;
	cout<<"SZ1000  : Steadily Move Z 1000 Pulses In(+)   [+1.0 inch]"<<endl;
	cout<<"SR600   : Steadily Move Z  600 Pulses CW(+)   [ +90 degs]"<<endl<<endl;


	cout<<"L0 : Set current location as new origin."<<endl;
	cout<<"LX : Read current X location."<<endl;
	cout<<"LY : Read current Y location."<<endl;
	cout<<"LZ : Read current Z location."<<endl;
	cout<<"LR : Read current R location."<<endl<<endl;

	cout<<"V1 : Enable verbose reply to M commands."<<endl;
	cout<<"V0 : Reply to M commands with only the pulses moved."<<endl<<endl;
}
