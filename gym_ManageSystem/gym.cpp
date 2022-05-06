#include <unistd.h>
#include <pthread.h>

#include <iostream>
#include "gym.hpp"

using namespace std;

int main()
{
  GymManageSystem* gm = new GymManageSystem();
  if(gm->InitMember() < 0)
  {
    cout << "GYM Init failed" << endl;
    return 0;
  }
  gm->StartGymSys();
  //gm->test();

  return 0;
}
