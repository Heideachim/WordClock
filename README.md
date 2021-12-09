
WordClock
=========

## Introduction
A clock not using a regular clock-face with numbers to display the time but plain words. 
If it is 2:15, this clock will display "It is a quarter past two". All the neccessary words
are already there, the appropriate ones will be highlichted. 

Unfortunately, this is highly dependant on the choosen language, here German. This not only
affects the clock-face, that would be easy to change. But also the structure of text. 
Here is an example in English: 
- 1:00 is: "It is one o'clock"
- 1:05 is: "It is five past one"
That is pretty regular, the hour 1 is always displayed as "one".

Compare this to German:
- 1:00 is: "Es ist ein Uhr"
- 1:05 is: "Es ist fünf nach eins"
Unfortunately, the hour 1 may get represented as "ein" or "eins". This needs to be handled in software and 
marks a deviation from other languages. 
Hence, we will continue in German, since that is the language this clock will serve time in.

An excellent overview about how to properly speak about time may be found on the web site
of the Deutsche Welle, Germany's international broadcaster: 
[Deutsch lernen: Uhrzeit (informell)](https://learngerman.dw.com/de/uhrzeit-informell-2/l-40443235/gr-40445046)



## Hardware 

- Frame: Ikea Ribba picture frame
- Reciever: DCF-77
- LEDs: WS2812 strip, 2m


