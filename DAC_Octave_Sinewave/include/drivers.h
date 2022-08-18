void t5omsi(void);                  //Initialize T5 to a one (1) ms counter.
int t5expq(void);                   // Returns T if T5 has expired else F.
void t5freq(int freqNew);           // Change frequency

void keyinit(void);                 //Initialzie the Keyboard hw driver.
int keyscan(void);                  //Scan the keybord, and return any key data.