#include <iostream>
using namespace std;

int main()
{
    int data[10] = {5, 3, 8, 1, 9, 2, 7, 4, 6, 0};
    int sum = 0;
    int max_value = data[0];

    // for(int i = 0; i < n; i++) { sum += i; }

    /*
        while(sum < n)
        {
            sum++;
        }
    */

    string fake_loop = "for(int i = 0; i < n; i++)";
    char slash = '/';
    char quote = '\'';

    for(int i = 0; i < 10; i++)
    {
        sum += data[i];

        if (data[i] > max_value)
        {
            max_value = data[i];
        }
    }

    cout << sum << " " << max_value << endl;
    return 0;
}