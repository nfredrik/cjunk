#include <iostream>
#include <vector>

//  g++ main.c

using namespace std;
int main()
{
    vector <int> example;         //Vector to store integers
    example.push_back(3);         //Add 3 onto the vector
    example.push_back(10);        //Add 10 to the end
    example.push_back(33);        //Add 33 to the end
    for(int x=0; x<example.size(); x++) 
    {
        cout<<example[x]<<" ";    //Should output: 3 10 33
    }
    cout << "\n";

    if(!example.empty())          //Checks if empty
        example.clear();          //Clears vector
    vector <int> another_vector;  //Creates another vector to store integers
    another_vector.push_back(10); //Adds to end of vector
    example.push_back(10);        //Same
    if(example==another_vector)   //To show testing equality
    {
        example.push_back(20); 
    }
    for(int y=0; y<example.size(); y++)
    {
        cout<<example[y]<<" ";    //Should output 10 20
    }
    cout << "\n";


    int i = example.at(5);        // throws an exeption!

    return 0;
}
