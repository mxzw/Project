#include "webgobang.hpp"

int main()
{
    WebGoBang* wgb = new WebGoBang();
    if(wgb == nullptr)
    {
        cout << "new failed" << endl;
        return 0;
    }

    wgb->Init();

    wgb->StartWebGoBang();

    

    delete wgb;
    return 0;
}
