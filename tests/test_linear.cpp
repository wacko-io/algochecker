#include <iostream>
using namespace std;

int main()
{
    int n = 100;
    int sum = 0;
    int positive_count = 0;

    // while(n > 0) { n--; }

    /*
        for(int j = 0; j < 500; j++)
        {
            sum += j;
        }
    */

    string text = "while(i < n)";
    char c = 'a';

    for(int i = 0; i < n; i++)
    {
        int value = i - 30;

        if (value > 0)
        {
            sum += value;
            positive_count++;
        }
    }

    cout << sum << " " << positive_count << endl;
    return 0;
}