#include <iostream>
using namespace std;

int main()
{
    int n = 10;

    // for(int i = 0; i < n; i++) { cout << i; }

    /*
       while(n > 0)
       {
           n--;
       }
    */

    string s = "for(int i=0;i<n;i++)";
    char c = '/';
    char q = '\'';

    for(int i = 0; i < n; i++)
    {
        cout << i << endl;
    }

    return 0;
}