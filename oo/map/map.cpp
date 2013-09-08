#include <iostream>
#include <string>
#include <map>
 
using namespace std;
 
int main()
{
    typedef map<string, int> mapType;
    mapType data;
 
    // let's declare some initial values to this map
    data["BobsScore"] = 10;
    data["MartysScore"] = 15;
    data["MehmetsScore"] = 34;
    data["RockysScore"] = 22;
    data["RockysScore"] = 23;  /*overwrites the 22 as keys are unique */
 
    // Iterate over the map and print out all key/value pairs.
    // Using a const_iterator since we are not going to change the values.
    for(mapType::const_iterator it = data.begin(); it != data.end(); ++it)
    {
        cout << "Who(key = first): " << it->first;
        cout << " Score(value = second): " << it->second << '\n';
    }
 
    return 0;
}


