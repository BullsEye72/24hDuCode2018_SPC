#define  C     3830    // 261 Hz
#define  D     3400    // 294 Hz
#define  E     3038    // 329 Hz
#define  F     2864    // 349 Hz 
#define  G     2550    // 392 Hz 
#define  A     2272    // 440 Hz 
#define  B     2028    // 493 Hz 

// Define a special note, 'R', to represent a rest
#define  R     20

void chanter(char* tab)
{
  long duration  = 0;
  int rest_count = 100;
  int speaker = A5;
  long tempo = 10000;
  int pause = 1000; 
  int tone_ = 0;
  int beat = 0;
  int songLength = sizeof(tab) / sizeof(char)-1;  
  int melody[songLength];
  int beats[songLength];

  pinMode(speaker, OUTPUT);
 
  Serial.print("ON EST ICI : ");
  Serial.println(songLength);

  for(int i = 0; i < songLength; i++)
  {
    if(tab[i] == 'C')
    {
      melody[i] = C;
      beats[i] = 32;
    }
    else if(tab[i] == 'D')
    {
      melody[i] = D;
      beats[i] = 32;
    }
    else if(tab[i] == 'E')
    {
      melody[i] = E;
      beats[i] = 32;
    }
    else if(tab[i] == 'F')
    {
      melody[i] = F;
      beats[i] = 32;
    }
    else if(tab[i] == 'G')
    {
      melody[i] = G;
      beats[i] = 32;
    }
    else if(tab[i] == 'A')
    {
      melody[i] = A;
      beats[i] = 32;
    }
    else if(tab[i] == 'B')
    {
      melody[i] = B;
      beats[i] = 32;
    }
    else if(tab[i] == '#')
    {
      melody[i] = R;
      beats[i] = 32;
    }
  }

  for (int i=0; i<songLength; i++) 
  {
    tone_ = melody[i];
    beat = beats[i];

    duration = beat * tempo;

    long elapsed_time = 0;
    if (tone_ > 0) 
    { 
      while (elapsed_time < duration) 
      {
        digitalWrite(speaker,HIGH);
        delayMicroseconds(tone_ / 2);

        // DOWN
        digitalWrite(speaker, LOW);
        delayMicroseconds(tone_ / 2);

      // Keep track of how long we pulsed
        elapsed_time += (tone_);
      } 
    }
    else 
    {
      for (int j = 0; j < rest_count; j++) 
      {
        delayMicroseconds(duration);  
      }                                
    } 
    
    delayMicroseconds(pause);
   } 
}

