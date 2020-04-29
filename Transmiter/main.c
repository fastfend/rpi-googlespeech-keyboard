#include <string>
#include <unistd.h>
#include <stdio.h>
#include <wiringPi.h>
#include <iostream>
#include <ctime>
#include <thread>
#include "process.h"
#include <wiringSerial.h>

using namespace procxx;
using namespace std;

int LED = 1;
int BUTTON = 23;

void QuickLEDBlink();
void SlowLEDBlink();
void Breaker();
bool AskGoogle();
bool Configure();
bool Record();
bool MainJob();

bool Working = false;
bool Working2 = false;
bool Pressed = false;

thread quickLedThread;
thread slowLedThread;
thread recordbreaker;

void QuickLEDBlink()
{
    cout << "[Thread] Wykonywanie w¹tku quickLED..." << endl;
	int num;
	pinMode (LED, PWM_OUTPUT);
	while (true)
	{
		if(Working)
		{
			for (num = 0 ; num < 1024 ; ++num)
			{
      			pwmWrite (LED, num) ;
      			usleep(50);
            }
    			usleep(10000);
    			for (num = 1023 ; num >= 0 ; --num)
    			{
      				pwmWrite (LED, num) ;
      				usleep(50) ;
	    		}
    			usleep(50000);
			}
			else
			{
				cout << "[Thread] Koñczenie w¹tku quickLED..." << endl;
				break;
			}
		}
}

void SlowLEDBlink()
{
    cout << "[Thread] Wykonywanie w¹tku slowLED..." << endl;
	int num;
	pinMode (LED, PWM_OUTPUT);
	while (true)
	{
		if(Working)
		{
			for (num = 0 ; num < 1024 ; ++num)
			{
      			pwmWrite (LED, num) ;
      			usleep(100);
            }
    			usleep(100000);
    			for (num = 1023 ; num >= 0 ; --num)
    			{
      				pwmWrite (LED, num) ;
      				usleep(100) ;
	    		}
    			usleep(100000);
			}
			else
			{
				cout << "[Thread] Koñczenie w¹tku slowLED..." << endl;
				break;
			}
		}
}

bool Configure()
{
    //Ports
    cout << "[Boot] Konfiguracja portÃ³w..." << endl;
    system("gpio mode 1 pwm; gpio mode 23 in; gpio mode 23 up");
    //WiringPi
    cout << "[Boot] Konfiguracja WiringPi..." << endl;
    if(wiringPiSetup() == -1)
	{
	cout << "[Boot] B³ad podczas uruchamiania WiringPi!" << endl;
	return false;
	}
	cout << "[Boot] WiringPi uruchomione..." << endl;

    //LED
    cout << "[Boot] Konfiguracja LED..." << endl;
    pinMode(LED, OUTPUT);
	digitalWrite(LED,0);	
	//Button
    cout << "[Boot] Konfiguracja przycisku..." << endl;
	pinMode(BUTTON,INPUT);
	pullUpDnControl(BUTTON, PUD_UP);

    cout << "[Boot] Konfiguracja zakoñczona" << endl;
    return true;
}

bool AskGoogle()
{
    cout << "[Google Cloud] Pytam Google o transkrypcjê..." << endl;
	int value = system("wget -q --post-file export.flac --header \"Content-Type: audio/x-flac; rate=44100\" -O - \"https://www.google.com/speech-api/v2/recognize?output=json&lang=pl-PL&key=###removedkey###\" > export.json");
    if (value != 0)
    {
        cout << "[Google Cloud] Google nie odpowiada..." << endl;
        return false;
    }
    cout << "[Google Cloud] Google odpowiada i znalaz³ rozwi¹zanie..." << endl;
    cout << "[Google Cloud] Przetwarzam dane z Google..." << endl;
    system("grep -Po '\"transcript\":.*?[^\\\\]\",' export.json > final_export.json");
    cout << "[Google Cloud] Zapytanie w pe³ni udane" << endl;
    return true;
}

bool Record()
{
    cout << "[Nagrywanie] Zaczynam nagrywanie..." << endl;    
    recordbreaker = thread(Breaker);
    Working2 = true;
    sleep(1);
    while(Working2)
    {
        if(digitalRead(BUTTON) == 0)
        {
            cout << "[Event] Klikniety przycisk 2x" << endl;
            Working2 = false;
            system("killall -s SIGINT arecord");
            break;
        }
    }
    recordbreaker.join();
    cout << "[Nagrywanie] Nagrywanie udane..." << endl;
    return true; 
}

void Breaker()
{
    system("arecord -q -D plughw:1,0 -d 5 -f S16_LE -r 44100 -c 1 -t wav | flac - -f --best -s -o export.flac");
}

bool MainJob()
{
    while(true)
	{	if(Pressed == false)
		{
			if(digitalRead(BUTTON) == 0)
			{
				Pressed = true;
				cout << "[Event] Klikniety przycisk" << endl;
				usleep(100);

                Working = true;
                slowLedThread = thread(SlowLEDBlink);
                if(Record() != true) return false;
                Working = false;
                slowLedThread.join();

                Working = true;
                quickLedThread = thread(QuickLEDBlink);
                if(AskGoogle() != true) return false;
                
                

				process read{"cat", "final_export.json"};
				cout << endl;
				cout << endl;
				cout << "Otrzymano:" << endl;
				read.exec();
				std::string line;
				std::string aras;
				std::string aras2;
				while (std::getline(read.output(), line))
				{
					std::string s = "\"transcript\":\"";
					string::size_type i = line.find(s);
					if (i != string::npos)
					aras = line.erase(i, s.length());

					std::string s2 = "\",";
					string::size_type i2 = aras.find(s2);
					if (i2 != string::npos)
					aras2 = aras.erase(i2, s2.length());
					
					std::string comm = "sshpass -p password ssh -o StrictHostKeyChecking=no pi@rpikeyboard.local /home/pi/save_text.sh \""+aras2+"\"";
					std::cout << aras2 << std::endl; 
					
					cout << "Wysy³am do klawiatury..." << endl;
					system(comm.c_str()); //SEND TO UART
					Working = false;
					quickLedThread.join();
				}
				read.close(pipe_t::write_end());
			}
		}
		else
		{
			if(digitalRead(BUTTON) == 1)
			{
				Pressed = false;
				usleep(100);
			}
		}
	}
}

int main()
{
    if (Configure() == false) return 1;
    if (MainJob() == false) return 2;
    return 0;
}
