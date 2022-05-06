#include <unistd.h>
#include <pthread.h>

#include <iostream>
#include "AdminPage.hpp"
#include "UserPage.hpp"

using namespace std;

void* AdminPagePthread(void* arg)
{
  GymManageSystem* gm = new GymManageSystem();
  if(gm->InitMember() < 0)
  {
    cout << "Admin Page Init failed" << endl;
    return NULL;
  }
  gm->StartGymSys();
  return NULL;
}

void* UserPagePthread(void* arg)
{
  UserManageSystem* um = new UserManageSystem();
  if(um->InitMember() < 0)
  {
    cout << "User Page Init failed" << endl;
    return NULL;
  }
  um->StartUserSys();
  return NULL;
}

int main()
{
  pthread_t pid[2];

  int ap = pthread_create(&pid[0],NULL,AdminPagePthread,NULL);
  if(ap < 0)
  {
    cout << "AdminPagePthread start failed ! " << endl;
    return 0;
  }

  int up = pthread_create(&pid[1],NULL,UserPagePthread,NULL);
  if(up < 0)
  {
    cout << "UserPagePthread start failed ! " << endl;
    return 0;
  }

  for(int i = 0; i < 2; ++i)
  {
    if(pthread_join(pid[i],NULL) < 0)
    {
      cout << "pthread_join failed ! " << endl;
      continue;
    }
    else{
      if(i == 0)
        cout << "join AdminPagePthread success ! " << endl;
      else
        cout << "join UserPagePthread success ! " << endl;
    }
  }

  return 0;
}
