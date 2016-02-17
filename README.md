Money Vacuum - A Donation Machine
=======

Money vacuum is an interactive exhibit at the [WOW kid's museum](http://www.wowchildrensmuseum.org/) in Lafayette, CO. Inserting a crumbled bill causes the machine to turn on, sucking the bill out of your hand and through a series of clear tubes. Once the bills reach the collection hopper a strobe light goes off and a short musical track or voice track (pirates thanking you) is played.

![A gif of sucking up money](https://raw.githubusercontent.com/nseidle/Money_Vacuum/master/Images/Money-Vacuum.gif)

The increased interactivity has increased donations from around $30 a month (a locked box marked 'Donate') to roughly $150 a month. It's wonderful to watch a parent pull out a dollar, let their kid crumple it up and send it through the system. The kid says 'AGAIN!' and the next smallest bill the parent has is $5...

![The series of tubes that form the donation machine](https://raw.githubusercontent.com/nseidle/Money_Vacuum/master/Images/Entire%20System.jpg)  

This is the entire system on the wall. The blower (vacuum) is located up in the rafters behind the wall in a noise dampening box. The brains are composed of two SSR controlled outlets connected to an Arduino + Electric Imp shield and are located in the rafters behind the wall. You can view the date feed [here](https://data.sparkfun.com/streams/dZNaDj6ZNgtZEm11vg19) on SparkFun's free data service.

![The intake tube](https://raw.githubusercontent.com/nseidle/Money_Vacuum/master/Images/System%20Intake.jpg)

The intake tube has two orthogonal IR beams that, when broken, turn on the vacuum.

![Photo instructions](https://raw.githubusercontent.com/nseidle/Money_Vacuum/master/Images/System%20Instructions.jpg)

It's important to crumple the bill before you insert the bill into the tube so that there is enough surface area for it to be sucked through the system. Flat bills tend to stick to the sides of the tubes.

![Collection Hopper](https://raw.githubusercontent.com/nseidle/Money_Vacuum/master/Images/Collection%20Hopper.jpg)

The collection hopper is located behind the front desk. It is made from a 5 gallon plastic brewing carboy that's had the bottom 1/3rd cut off and the body of the carboy cut vertically and taken in like you would an over-sized shirt. The original bottom piece is then friction fit back onto the upper portion. The suction is enough that this interface is not nearly air-tight but works great. This interface was designed to be easy to open by floor staff. Because of the volume of donations (this is awesome!) they need to empty the hopper once a day before it becomes a security/theft risk. One downside is that they have to spend a bit of time flattening bills, though I have not heard any complaints.

Note: There is an on/off switch below the hopper (shown at bottom of photo). This is a toggle switch with LED that allows the floor staff to disable the intake gate if they are too busy to monitor the system. If the switch is moved down, the LED turns off and the system ignores any activity at the intake. This is basically an override in case traffic on the floor gets too hectic to monitor 6 year old trying to suck their little brother's sock into the system (won't hurt but is not good).

![Tube hooks](https://raw.githubusercontent.com/nseidle/Money_Vacuum/master/Images/3%20Sets%20of%20Tube%20Hooks.jpg)

The series of clear acrylic tubes are supported by three sets of [hose hangers](http://www.amazon.com/gp/product/B00FAMGDSG).

![Installed Electrical System](https://raw.githubusercontent.com/nseidle/Money_Vacuum/master/Images/Control%20Board%20Installed.jpg)

There are two SSR controlled outlets for the vacuum/dust collection blower (AC) and strobe light (AC). There is an amplifier to increase the audio signal coming from the SparkFun MP3 shield. The controller is a [SparkFun RedBoard Arduino](https://www.sparkfun.com/products/12757) with the [MP3 shield](https://www.sparkfun.com/products/12660), an [Electric Imp shield](https://www.sparkfun.com/products/12887) and a [proto shield](https://www.sparkfun.com/products/7914) with [RJ45 connectors](https://www.sparkfun.com/products/716) to the various IR gates. If I had to do it again today I would use the [ESP8266 Thing Dev Board](https://www.sparkfun.com/products/13711).

![Noise Cancellation](https://raw.githubusercontent.com/nseidle/Money_Vacuum/master/Images/Noise%20Supression%20Box.jpg)

After installing the system the floor staff asked that we dampen the noise of the blower. Mind you, the floor of the kid's museum is absolute mayhem every day of the week. The blower was being activated quite often (with and without donations) and the din was getting to the wonderful folks that run the museum day to day. So I designed a box that could be inserted and assembled into the rafters. Made of 2x4' sheets of foam and duct tape, I installed an attic vent as the exhaust port (a circle cut into a 2x2 foam board). It worked well but the blower pushed enough air into this box to blow it out. I reinforced the box with hand bent metal straps found in the drywall isle (not sure what their actual usage is). This solution has dampened the noise considerably (but it's still loud) for the last three years. If I had more space between the rafters I would assemble a proper box on the ground and then install it; but otherwise, this is a great solution.

Bill of Materials
-------------------

This is a rough list of materials and vendors:

**Vacuum System:**

* [Central Machinery, 1HP Dust collection system](http://www.amazon.com/gp/product/B006ZBCF0Q): $104
* [4" clear acrylic tube](http://www.delviesplastics.com/p/Round_Extruded_Tube.html) (48" lengths): 44 linear ft, $15 per foot
* [4" Flexible Clear Hose](http://www.amazon.com/gp/product/B001DT15GK) x 30 ft: $31 per 10'
* [4" J-Hooks 5-Pack](http://www.amazon.com/gp/product/B001C06B5K) x 15pcs : $22 per pack
* [4" Hose clamps 5-Pack](http://www.amazon.com/gp/product/B002F1H01E) - 20pcs: $10 per pack
* [4" Hose Hangers 6-Pack](http://www.amazon.com/gp/product/B00FAMGDSG): $11
* [Revolving Beacon Light](http://www.amazon.com/gp/product/B0011CZV5A): $12
* [6 gal PET Carboy](http://www.amazon.com/dp/B00FKA4BFK): $25
* [4" Connector Collars](http://www.amazon.com/gp/product/B00AX5I9XM): $10 for pack of 2
* [3M Transparent Weather Sealing Tape](http://www.amazon.com/gp/product/B0000CBIFF): $4

**Electronics:**

* [Solid State Relay](https://www.sparkfun.com/products/13015) x 2pcs : $10 per relay
* Double Gang Metal Housing - 2pcs: $5 per housing from local hardware store
* Double Gang Metal Plate - 2pcs: $1 from local hardware store
* Outlet - 2pcs: $1 per outlet from local hardware store
* Power Strip: $8 You absolutely have one of these sitting around
* Wooden Backer Board: Roughly 1' x 2'
* [Mini Amplifier](https://www.amazon.com/s/ref=nb_sb_noss?field-keywords=mini+amplifier): $20
* [Dual Indoor/Outdoor Speakers](http://www.amazon.com/gp/product/B000A5S926): $30 for the pair but you only need one

**Control System:**

* [RedBoard](https://www.sparkfun.com/products/12757): $20
* [5V Wall Adapter](https://www.sparkfun.com/products/12889): $6
* [MP3 Player Shield](https://www.sparkfun.com/products/12660): $35
* [Electric Imp](https://www.sparkfun.com/products/11395): $30
* [Electric Imp Shield](https://www.sparkfun.com/products/12887): $20
* [Protoshield](https://www.sparkfun.com/products/7914): $10
* [RJ45 connectors](https://www.sparkfun.com/products/716) x 2: $1
* [RJ45 Breakout](https://www.sparkfun.com/products/716) x 2: $2
* [2-Pin JST](https://www.sparkfun.com/products/9914) x 2: $1
* [LED switch](https://www.sparkfun.com/products/11310): $3
* [IR LED](https://www.sparkfun.com/products/9349) x 2: $1
* [IR Receiver](https://www.sparkfun.com/products/10266) x 2: $2

**Noise Supression Box:**

* 4x2' Sheet of 2" Styrofoam Board x 5
* Exhaust vent
* Duct tape
* Bent Metal Straps (Something with lots of surface area and bendable by hand)

You might also need a fish to get the cables where they need to be inside the walls.

License Information
-------------------

The hardware design and firmware are released under [Creative Commons Share-alike 3.0](http://creativecommons.org/licenses/by-sa/3.0/).  

Feel free to create your own copy of the Money Vacuum. All I ask is that you include attribution of 'Based on the Money Vacuum by nseidle'.

The donation machine was inspired by a video I found online that featured the L.A. based New Children's Museum. They have a very similar donation system built by [Team Pneumo](http://teampneumo.com/). Checkout [their video](https://vimeo.com/73435892). So cool! I took their idea and mixed in my own bits. All credit goes to them.

Repository Contents
-------------------
* **/Master Plan** - Contains a wiring diagram of all the bits of the system
* **/Sound Files** - The MP3s that play randomly once a bill has reached the collection hopper (provided by the WOW musem)
* **/Firmware** - 
	* Money_Vacuum.ino - The firmware that runs on an Arduino Uno that controls everything
    * agent.nut - The code that runs in the cloud via Electric Imp that communicates to data.sparkfun.com logging service so I can see when
	* device.nut - The code that runs locally to the Electric Imp and talks to Money_Vacuum.ino