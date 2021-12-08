#include <iostream>
#include <string>   // C++ STL

using namespace std;

int main()
{
    cout << "Hallo WortUhr!" << endl;

    string fuenf = "fünf ";
    string zehn = "zehn ";
    string viertel = "viertel ";
    string zwanzig = "zwanzig ";
    string no = "";
    string vor = "vor ";
    string nach = "nach ";
    string halb = "halb ";
    string uhr = " Uhr";

                //             0       5       10      15       20       25      30      35      40       45       50      55
    string fuenfer[] = {       no,     fuenf,  zehn,   viertel, zwanzig, fuenf,  no,     fuenf,  zwanzig, viertel, zehn,   fuenf};
    string wohin[] =   {       no,     nach,   nach,   nach,    nach,    vor,    no,     nach,   vor,     vor,     vor,    vor};
    string halbe[] =   {       no,     no,     no,     no,      no,      halb,   halb,   halb,   no,      no,      no,     no};
    // string uhren[] =   {       uhr,    "",     "",     "",      "",      "",     "",     "",     "",      "",      uhr,    uhr};
    int richtung[] =   {       0,      0,      0,      0,       0,       1,      1,      1,      1,       1,       1,      1};
    string stunden[] = {"ein", "eins", "zwei", "drei", "vier", "fünf", "sechs", "sieben", "acht", "neun", "zehn", "elf", "zwölf"};

    for (int h = 1; h < 3; h++) {
        for (int m = 0; m < 60; m += 5) {
            int i = m/5;
            string s;
            s += fuenfer[i];
            s += wohin[i];
            s += halbe[i];
            if ((h == 1) && (i == 0))
                s += stunden[0];        // sonderfall: 'ein Uhr' statt 'eins Uhr'
            else
                s += stunden[h + richtung[i]];
            if (i == 0)                 // nur bei vollen stunden wird 'Uhr' angehängt
                s += uhr;
            cout << "Es ist " << s << "." << endl;
        }
    }

}