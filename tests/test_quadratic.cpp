#include <iostream>
using namespace std;

int main()
{
    int n = 100;
    int sum = 0;

    // for(int x = 0; x < 1000; x++) { sum += x; }

    /*
        string hidden = "for(int i = 0; i < n; i++)";
        while(sum < n)
        {
            sum++;
        }
    */

    string fake = "while(j < n)";
    char slash = '/';

    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            int value = i + j;
            sum += value;
        }
    }

    cout << sum << endl;
    return 0;
}