#include <iostream>
#include <random>
#include <thread>
#include <list>
#include <map>
#include <regex>
#include <stdio.h>
#include <ncursesw/ncurses.h>
#include <mutex>
#include <locale.h>


#define CLS "\x1B["
#define KB_LEFT 75
#define KB_RIGHT 77


std::mutex paintRoad;

using namespace std;

void clearScreen()
{
#ifdef _WIN32
    printf(CLS "2J");
#elif __linux__
    clear();
    refresh();
#elif __unix__
    printf(CLS "2J" CLS "H");
#endif
}

int playerLine;
int fieldLenght;
int fieldWight;
int score;
int speedUp=0;
int curSpeed=1;

std::vector<std::list<int> > listOfLines;


const std::string oneSeg("|     ");
std::string seg1("  @  ");
std::string seg2(" @@@ ");
std::string seg3("  @  ");
std::string seg4(" @ @ ");

const std::map<int,std::string> listCar{{1,seg1},{2,seg2},{3,seg3},{4,seg4}};

std::map<int,std::list<int> > mapOfAllCarsNoses;

const std::string lastSeg("|     |\n");

std::list<std::string> roadFull;

void printScore(){
    printw("Скорость : ");
    printw(to_string( curSpeed).c_str());
    printw("\nСчет : ");
    printw(to_string( score).c_str());
    printw("\n");
}

void addOneLine(bool isLastLine,int lines, const std::list<int> &carNoses){
    std::string retVal;
    std::string replaced;
    std::string currSeg= isLastLine?lastSeg:oneSeg;
    if(roadFull.size()==0)
        for(int i=0;i<lines;i++){
        roadFull.push_back("");
        }
    int tempLines;
    std::map<int,std::string> fullCars;
    for(auto itCarPos:carNoses)
    {
        tempLines=itCarPos;
        for(auto itListCar:listCar)
        {
            fullCars.insert(std::make_pair(tempLines--,itListCar.second));
        }
    }
    tempLines=roadFull.size();
    std::list<std::string> roadFullTemp;
    for (auto itRoad:roadFull)
    {
        if(fullCars.find(tempLines)!=fullCars.end()){
            replaced=std::regex_replace(currSeg, std::regex("     "), fullCars.at(tempLines) );
            itRoad.append(replaced);
            roadFullTemp.push_back(itRoad);
        }
        else{
            itRoad.append(currSeg);
        roadFullTemp.push_back(itRoad);
        }
        --tempLines;
    }
    roadFull=roadFullTemp;
    return;
}


#include <unistd.h>


void getAnim(int fWight, int fLenght)
{
    /*
    int firstLine=17, secondLine=8;
    while(true)
    {
        clearScreen();
        std::list<int> carNoses1{firstLine};
        std::list<int> carNoses2{secondLine};
        if (--firstLine<=0) firstLine=19+5;
        if (--secondLine<=0) secondLine=19+5;
        addOneLine(0,19, carNoses1);
        addOneLine(1,19, carNoses2);
        for(auto it:roadFull){
            cout<<it;
        }
        roadFull.clear();
        usleep(100000);
    }
    */
    fieldWight=fWight;
    fieldLenght=fLenght;
    playerLine=0;
    score=0;
    int currPosCar=4;
    std::list<int> emptyList;
    std::list<int> removeList;
    for(int i=0;i<fWight;i++){
        listOfLines.push_back(emptyList);
    }
    listOfLines.at(0).push_back(currPosCar);
    int tempLine=0;
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> distLines(0,fWight-1);
    std::uniform_int_distribution<std::mt19937::result_type> distCars(5+4,5+7);
    currPosCar+=distCars(rng);
    listOfLines.at(distLines(rng)).push_back(currPosCar);
    while(true){
        ++score;
        paintRoad.lock();
        tempLine=0;
        if(currPosCar<fLenght){
            currPosCar+=distCars(rng);
            listOfLines.at(distLines(rng)).push_back(currPosCar);
        }
        for(auto &it:listOfLines){
            for(auto &it1:it){
                if(tempLine==playerLine&&it1<5+4)
                {
                    int count=0;
                    for(auto &it2:it)
                    {
                        if(it2<5+4&&it2>0) count++;
                        if (count>1) {
                            endwin();
                            cout<<"А всё!"<<endl;
                            cout<<"Счет : "<<score<<endl;
                            exit(0);
                        }
                    }
                }
                else
                {
                    --it1;
                    if (it1<0) removeList.push_back(it1);
                }
            }
            for(auto it1:removeList){
                it.remove(it1);
            }
            removeList.clear();
             if(tempLine++==listOfLines.size()-1)
                 addOneLine(1,fLenght, it);
             else
                 addOneLine(0,fLenght, it);

        }
        clearScreen();
        for(auto it:roadFull){
             printw(it.c_str());
        }
        --currPosCar;
        printScore();
        refresh();
        roadFull.clear();
        paintRoad.unlock();
        usleep(300000-curSpeed*25000-speedUp);
        if(score%50==0&&curSpeed<10)
            ++curSpeed;
    }

}



void moveCar(int pos)
{
    if(pos<0||pos>fieldWight-1) return;
    paintRoad.lock();
    int tempLine=0;
    std::list<int>::iterator itInt;
    roadFull.clear();
    for(auto &it:listOfLines){
        itInt=it.begin();
        for(auto &it1:it){
            if(tempLine==playerLine&&it1<5+4)
            {
                int count=0;
                for(auto &it2:it)
                {
                    if(it2<5+4&&it2>0) count++;
                    if (count>1) {
                        endwin();
                        cout<<"А всё!"<<endl;
                        cout<<"Счет : "<<score<<endl;
                        exit(0);
                    }
                }
            }

        }
        while(itInt != it.end())
        {
            if(*itInt<5+4&&tempLine==playerLine) it.erase(itInt++);
            else
                itInt++;
        }
        if(tempLine==pos)it.push_back(4);

         if(tempLine==fieldWight-1)
             addOneLine(1,fieldLenght, it);
         else
             addOneLine(0,fieldLenght, it);
         tempLine++;
    }
    playerLine=pos;
    clearScreen();
    for(auto it:roadFull){
        printw(it.c_str());
    }
    printScore();
    refresh();
    roadFull.clear();
    paintRoad.unlock();
}



void getKB()
{
    int KB_KODE=0;
    int ch=0;
    /*
    tcgetattr( STDIN_FILENO, &oldt);
    newt=oldt;
    newt.c_iflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);*/
    while(ch!=27){

    ch=getch();
    switch (ch) {
    case KEY_LEFT:
        moveCar(playerLine-1);speedUp=0;
        break;
    case KEY_RIGHT:
        moveCar(playerLine+1);speedUp=0;
        break;
    case KEY_UP:
        speedUp=50000;
        break;
    }
    }
    endwin();
    cout<<"Сбежал, трус!"<<endl;
    cout<<"Счет : "<<score<<endl;
    exit(0);
}

int main(int argc, char* argv[])
{
    setlocale(LC_ALL,"");
    int wight=2;
    if(argc==2) wight=stoi(argv[1]);
    initscr();
    raw();
    keypad(stdscr, TRUE);
    set_escdelay(0);
    noecho();
    std::thread tK(getAnim,wight,20);
    tK.detach();
    std::thread tM(getKB);
    tM.join();

    return 0;
}
