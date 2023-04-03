
// const int ch_MAX = 26;
// string RandomString(int ch)
// {
//     string  alpha[ch_MAX] = { "mery ", "lalouli ", "1337", " ana"};
//     string result = "";
//     for (int i = 0; i<ch; i++)
//         result = result + alpha[rand() % ch_MAX];

//     return result;
// }
// int main()
// {
//    srand(time(NULL));
//    siz ch = 15;
//    cout<<RandomString(ch) <<"\n";
//    return 0;
// }


// #include <iostream>
// #include <string>
// #include <cstdlib> // for rand() and srand()
// #include <ctime> // for time()

// using namespace std;

// void random_message() {
//     // Create an array of messages
//     string messages[] = {
//         "Hello!",
//         "How are you?",
//         "Nice to meet you!",
//         "What can I do for you?",
//         "Have a great day!",
//         "See you soon!"
//     };
    
//     // Calculate the number of messages in the array
//     int num_messages = sizeof(messages) / sizeof(messages[0]);
    
//     // Generate a random index between 0 and the number of messages
//     srand(time(nullptr)); // Initialize the random number generator
//     int index = rand() % num_messages;
    
//     // Return the random message
//     cout << messages[index] << endl;
// }

// int main() {
    
//     // Get the recipient's name from the user
   
    
//     // Generate a random message
//     random_message();
    
//     // Send the message to the recipient
    
//     return 0;
// }