#include <iostream>
#include "gym.hpp"

using namespace std;

int main()
{
  GymManageSystem* gm = new GymManageSystem();
  gm->InitMember();
  gm->StartGymSys();
  // gm->test();

  return 0;
}
