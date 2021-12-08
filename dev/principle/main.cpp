#include <iostream>
#include <string>   // C++ STL

using namespace std;

int main()
{
    cout << "Hallo Wort-Uhr!" << endl;

    string fuenf = "fünf ";
    string zehn = "zehn ";
    string viertel = "Viertel ";
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
    string stunden[] = {"", "eins", "zwei", "drei", "vier", "fünf", "sechs", "sieben", "acht", "neun", "zehn", "elf", "zwölf", "eins"};  // ein zusätzlicher Eintrag, weil auch 12+1 abgefragt wird
    string stunden_ein = "ein";     // Sonderfall 'ein Uhr' (nicht 'eins Uhr')

    for (int H = 23; H < 24; H++) {
        for (int M = 50; M < 60; M += 1) {
            int h = H;
            int m = (M+2)/5;                // Minuten auf Fünfminuten runden: 0, 1, 2 --> 0; 3, 4, 5 --> 5; etc. 
            if (M >= 58) {                   // Sonderfall: alles 2 Minuten vor einer ganzen Fünfminutenzahl wird auf diese aufgerundet, führt beo 58+59 zu einem Stunden-Überlauf!
                m = 0;
                h = H + 1;
            }

            while (h >= 13)                 // Nachmittags-Zeiten auf vormittags mappen
                h -= 12;           
            if (h == 0)                     // Sonderfall: Mitternacht wird als zwölf Uhr vereinfacht            
                h = 12;

            string s;
            s += fuenfer[m];
            s += wohin[m];
            s += halbe[m];
            if ((h == 1) && (m == 0))
                s += stunden_ein;        // Sonderfall: 'ein Uhr' statt 'eins Uhr'
            else
                s += stunden[h + richtung[m]];
            if (m == 0)                  // nur bei vollen Stunden wird 'Uhr' angehängt
                s += uhr;
            cout << (H < 10 ? "0" : "") << H << (M < 10 ? ":0" : ":") << M << " - " << s << "." << endl;
        }
    }

}