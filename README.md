Money Vacuum - A Donation Machine
=======
Money vacuum is an interactive exhibit at the [WOW kid's museum](http://www.wowchildrensmuseum.org/) in Lafayette, CO . Inserting a crumbled bill causes the machine to turn on, sucking the bill out of your hand and through a series of clear tubes. Once the bills reach the collection hopper a strobe light goes off and a short musical track or voice track (pirates thanking you) is played.

The increased interactivity has increased donations (a locked box marked 'Donate') from around $30 a month to roughly $150 a month. It's wonderful to watch a parent pull out a dollar, let their kid crumple it up and send it through the system. The kid says 'AGAIN!' and the next smallest bill the parent has is $5...

![The series of tubes that form the donation machine](https://raw.githubusercontent.com/nseidle/Money_Vacuum/master/Images/Entire%20System.jpg)  

This is the entire system on the wall. The blower (vacuum) is located up in the rafters behind the wall in a noise dampening box. The brains are composed of two SSR controlled outlets connected to an Arduino + Electric Imp shield and are located in the rafters behind the wall.

![The intake tube](https://raw.githubusercontent.com/nseidle/Money_Vacuum/master/Images/System%20Intake.jpg)

The intake tube has two orthogonal IR beams that, when broken, turn on the vacuum.

![Photo instructions](https://raw.githubusercontent.com/nseidle/Money_Vacuum/master/Images/System%20Instructions.jpg)

It's important to crumple the bill before you insert the bill into the tube so that there is enough surface area for it to be sucked through the system. Flat bills tend to stick to the sides of the tubes.

![Collection Hopper](https://raw.githubusercontent.com/nseidle/Money_Vacuum/master/Images/Collection%20Hopper.jpg)

The collection hopper is located behind the front desk. It is made from a 5 gallon plastic brewing carboy that's had the bottom 1/3rd cut off and the body of the carboy cut vertically and taken in like you would an over-sized shirt. The original bottom piece is then friction fit back onto the upper portion. The suction is enough that this interface is not nearly air-tight but works great. This interface was designed to be easy to open by floor staff. Because of the volume of donations (this is awesome!) they need to empty the hopper once a day before it becomes a security/theft risk. One downside is that they have to spend a bit of time flattening bills, though I have not heard any complaints.

Note: There is an on/off switch below the hopper. This is a toggle switch with LED that allows the floor staff to disable the intake gate if they are too busy to monitor the system. This is basically an override in case traffic on the floor gets too hectic to monitor 6 year old trying to suck their little brother's sock into the system (won't hurt but is not good).

![Installed Electrical System](https://raw.githubusercontent.com/nseidle/Money_Vacuum/master/Images/Control%20Board%20Installed.jpg)

There are two SSR controlled outlets for the vacuum/dust collection blower (AC) and strobe light (AC). There is an amplifier to increase the audio signal coming from the SparkFun MP3 shield. The controller is a [SparkFun RedBoard Arduino](https://www.sparkfun.com/products/12757) with the [MP3 shield](https://www.sparkfun.com/products/12660), an [Electric Imp shield](https://www.sparkfun.com/products/12887) and a [proto shield](https://www.sparkfun.com/products/7914) with [RJ45 connectors](https://www.sparkfun.com/products/716) to the various IR gates. If I had to do it again today I would use the [ESP8266 Thing Dev Board](https://www.sparkfun.com/products/13711).

![Noise Cancellation](https://raw.githubusercontent.com/nseidle/Money_Vacuum/master/Images/Noise%20Supression%20Box.jpg)

After installing the system the floor staff asked that we dampen the noise of the blower. Mind you, the floor of the kid's museum is absolute mayhem every day of the week. The blower was being activated quite often (with and without donations) and the din was getting to the wonderful folks that run the museum day to day. So I designed a box that could be inserted and assembled into the rafters. Made of 2x4' sheets of foam and duct tape, I installed an attic vent as the exhaust port (a circle cut into a 2x2 foam board). It worked well but the blower pushed enough air into this box to blow it out. I reinforced the box with hand bent metal straps found in the drywall isle. This solution has dampened the noise considerably (but its still loud) for the last three years. If I had more space between the rafters I would assemble a proper box on the ground and then install it; but otherwise, this is a great solution.

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