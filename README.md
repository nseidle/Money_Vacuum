Money Vacuum - A Donation Machine
=======

Money vacuum is an interactive exhibit at a kid's museum. Inserting a crumbled bill causes the machine to turn on sucking the bill out of your hand and through a series of clear tubes. Once the bills reach the collection hopper a strobe light goes off and a short musical track or voice track (pirates thanking you) is played.

![The series of tubes that form the donation machine](https://raw.githubusercontent.com/nseidle/)  

License Information
-------------------

The hardware design and firmware are released under [Creative Commons Share-alike 3.0](http://creativecommons.org/licenses/by-sa/3.0/).  

Feel free to use, distribute, and sell varients of BeatBag. All I ask is that you include attribution of 'Based on BeatBag'.

The donation machine was inspired by some video I found online that feature the LA base New Children's Museum. They have a very similar donation system built by [Team Pneumo](http://teampneumo.com/). Checkout [their video](https://vimeo.com/73435892)! So cool. I took their idea and mixed in my own bits. All credit goes to them.

Repository Contents
-------------------
* **/Master Plan** - Contains a wiring diagram of all the bits of the system
* **/Sound Files** - The MP3s that play randomly once a bill has reached the collection hopper (provided by the WOW musem)
* **/Firmware** - 
	* Money_Vacuum.ino - The firmware that runs on an Arduino Uno that controls everything
    * agent.nut - The code that runs in the cloud via Electric Imp that communicates to data.sparkfun.com logging service so I can see when
	* device.nut - The code that runs locally to the Electric Imp and talks to Money_Vacuum.ino