#include <iostream>
using namespace std;

int main()
{
    int n = 50;
    int sum = 0;
    int extra = 0;

    // for(int k = 0; k < 10; k++) { sum += k; }

    /*
        for(int a = 0; a < n; a++)
        {
            while(sum < n)
            {
                sum++;
            }
        }
    */

    string fake = "for(int z = 0; z < n; z++)";
    char q = '\'';

    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            int k = 0;

            while(k < n)
            {
                sum += k;
                k++;
            }
        }
    }

    cout << sum + extra << endl;
    return 0;
}