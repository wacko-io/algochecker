#include <iostream>
using namespace std;

int main()
{
    int n = 100;
    int sum = 0;

    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            sum += j;
        }
    }

    return 0;
}