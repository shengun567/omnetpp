/**
@mainpage TicToc Tutorial for @opp

This short tutorial to @opp guides you through an example
of modeling and simulation, showing you along the way some
of the commonly used @opp features.

The tutorial is based on the Tictoc example simulation, which you
can find in the <tt>samples/tictoc</tt> directory of your
@opp installation, so you can try out immediately how
the examples work. However, you'll find the tutorial much more useful
if you actually carry out at least the first steps described here.

@note We assume here that you have a working @opp installation.
We also assume that you have a good C++ knowledge, and you are in general
familiar with C/C++ development (editing source files, compiling, debugging etc.)
on your operating system. (The latter two are out of our scope here --
there are excellent books, tutorials on the web, etc. if you need
to update your knowledge on that.)

To make the examples easier to follow, all source code in here is
cross-linked to the @opp API documentation.

This document and the TicToc model are an expanded version of
the original TicToc tutorial from Ahmet Sekercioglu (Monash University).

@section contents Contents

  - @ref part1
  - @ref part2
  - @ref part3
  - @ref part4
  - @ref part5

NEXT: @ref part1
*/

--------------------------------------------------------------------------

/**
@page part1 1. Getting started

UP: @ref contents

Since the most common application area of @opp is the simulation of
telecommunications networks, we'll borrow our topic from there.
For a start, let us begin with a "network" that consists of two nodes.
The nodes will do something simple: one the nodes will create a packet,
and the two nodes will keep passing the same packet back and forth.
We'll call the nodes "tic" and "toc".

Here are the steps you take to implement your first simulation from scratch:

1. Create a working directory called tictoc, and cd to this directory.

2. Describe your example network by creating a topology file. A topology file is a
text file that identifies the network's nodes and the links between them.
You can create it with your favourite text editor.
Let's call it tictoc1.ned:

@include tictoc1.ned

The file is best read from the bottom up. Here's what it says:

   - we define a network called tictoc1, which is an instance the
     module type Tictoc1 (<tt>network..endnetwork</tt>);
   - Tictoc1 is a compound module, which is assembled from two submodules,
     tic and toc. tic and toc are instances of the same module type
     called Txc1. We connect tic's output gate (named out) to toc's input
     gate (named in), and vica versa (<tt>module..endmodule</tt>). There
     will be a 100ms propagation delay both ways;
   - Txc1 is a simple module type (which means it is atomic on NED level, and
     will be implemented in C++). Txc1 has one input gate named in,
     and one output gate named out (<tt>simple..endsimple</tt>).

3. We now need to implement the functionality of the simple module Txc1. This is
achieved by writing a C++ file txc1.cc:

@include txc1.cc

The Txc1 simple module type is represented by the C++ class Txc1, which
has to be subclassed from cSimpleModule, and registered in @opp with the
Define_Module() macro. We redefine two methods from cSimpleModule: initialize()
and handleMessage(). They are invoked from the simulation kernel:
the first one only once, and the second one whenever a message arrives at the module.

In initialize() we create a message object (cMessage), and send it out
on gate <tt>out</tt>. Since this gate is connected to the other module's
input gate, the simulation kernel will deliver this message to the other module
in the argument to handleMessage() -- after a 100ms propagation delay
assigned to the link in the NED file. The other module just sends it back
(another 100ms delay), so it will result in a continuous ping-pong.

Messages (packets, frames, jobs, etc) and events (timers, timeouts) are
all represented by cMessage objects (or its subclasses) in @opp.
After you send or schedule them, they will be held by the simulation
kernel in the "scheduled events" or "future events" list until
their time comes and they are delivered to the modules via handleMessage().

Note that there is no stopping condition built into this simulation:
it would continue forever. You will be able to stop it from the GUI.
(You could also specify a simulation time limit or CPU time limit
in the configuration file, but we don't do that in the tutorial.)

4. We now create the Makefile which will help us to compile and link our program
to create the executable tictoc:

\code
$ opp_makemake
\endcode

This command should have now created a Makefile in the working directory
tictoc.

@note Windows+MSVC users: the command is <tt>opp_<i>n</i>makemake</tt>,
and it will create <tt>Makefile.vc</tt>.

5. Let's now compile and link our very first simulation by issuing the make command:

\code
$ make
\endcode

@note Windows+MSVC: type <tt>nmake -f Makefile.vc</tt>.
If you get <i>'nmake' is not recognized as an internal or external command...</i>,
find <tt>vcvars32.bat</tt> somewhere in the MSVC directories, and run it first thing
in every command window in which you want to compile.

If there are compilation errors, you need to rectify those and repeat the make until
you get an error-free compilation and linking.

6. If you start the executable now, it will complain that it cannot find
the file omnetpp.ini, so you have to create one. omnetpp.ini tells the
simulation program which network you want to simulate (yes, several networks
can live in the same simulation program), you can pass parameters
to the model, explicitly specify seeds for the random number generators etc.

Create the following very simple omnetpp.ini:

\code
[General]
network = tictoc1
\endcode

tictoc2 and further steps will all share the following omnetpp.ini:

@include omnetpp.ini

which even doesn't specify the network (the simulation program ask it
in a dialog when it starts).

7. Once you complete the above steps, you launch the simulation by issuing this
command:

\code
$ ./tictoc
\endcode

and hopefully you should now get the @opp simulation window.

@note Windows: the command is just <tt>tictoc</tt>.

8. Press the Run button on the toolbar to start the simulation. What you should
see is that tic and toc are exchanging messages with each other.

<img src="step1.gif">

The main window toolbar displays the simulated time. This is virtual time, it
has nothing to do with the actual (or wall-clock) time that the program takes to
execute. Actually, how many seconds you can simulate in one real-world second
depends highly on the speed of your hardware and even more on the nature and
complexity of the simulation model itself.

Note that it takes zero simulation time for a node to process the message.
The only thing that makes the simulation time pass in this model is
the propagation delay on the connections.

9. You can play with slowing down the animation or making it faster with
the slider at the top of the graphics window. You can stop the simulation
by hitting F8 (equivalent to the STOP button on the toolbar), single-step
through it (F4), run it with (F5) or without (F6) animation.
F7 (express mode) completely turns off tracing features for maximum speed.
Note the event/sec and simsec/sec gauges on the status bar of the
main window.

10. You can exit the simulation program by clicking its Close icon or
choosing File|Exit.

NEXT: @ref part2
*/

--------------------------------------------------------------------------

/**
@page part2 2. Enhancing the 2-node TicToc

PREV: @ref part1 UP: @ref contents

@section s2 Step 2: Refining the graphics, and adding debugging output

Here we make the model look a bit prettier in the GUI. We assign
the "block/process" icon (the file <tt>bitmaps/block/process.gif</tt>), and paint it cyan for tic
and yellow for toc. This is achieved by adding display strings to the
NED file. The <tt>i=</tt> tag in the display string specifies the icon.

@dontinclude tictoc2.ned
@skip submodules
@until connections

You can see the result here:

<img src="step2a.gif">

We also modify the C++ file to add some debug messages to Txc1 by
writing to the @opp <tt>ev</tt> object like this:

@dontinclude txc2.cc
@skipline ev <<

and

@skipline ev <<

When you run the simulation in the @opp GUI Tkenv, the following output
will appear in the main text window:

<img src="step2b.gif">

You can also open separate output windows for tic and toc by right-clicking
on their icons and choosing Module output from the menu. This feature
will be useful when you have a large model ("fast scrolling logs syndrome")
and you're interested only in the log messages of specific module.

<img src="step2c.gif">

Sources: @ref tictoc2.ned, @ref txc2.cc, @ref omnetpp.ini


@section s3 Step 3: Adding state variables

In this step we add a counter to the module, and delete the message
after ten exchanges.

We add the counter as a class member:

@dontinclude txc3.cc
@skip class Txc3
@until public:

We set the variable to 10 in initialize() and decrement in handleMessage(),
that is, on every message arrival . After it reaches zero, the simulation
will run out of events and terminate.

Note the

@dontinclude txc3.cc
@skipline WATCH(c

line in the source: this makes it possible to see the counter value
in Tkenv. Double-click on tic's icon, then choose the Contents page
from the inspector window that pops up.

<img src="step3.gif">

As you continue running the simulation, you can follow as the counter
keeps decrementing until it reaches zero.

Sources: @ref tictoc3.ned, @ref txc3.cc, @ref omnetpp.ini


@section s4 Step 4: Adding parameters

In this step you'll learn how to add input parameters to the simulation:
we'll turn the "magic number" 10 into a parameter.

Module parameters have to be declared in the NED file. The data type can
be numeric, string, bool, or xml (the latter is for easy access to
XML config files), among others.

@dontinclude tictoc4.ned
@skip simple
@until gates

We also have to modify the C++ code to read the parameter in
initialize(), and assign it to the counter.

@dontinclude txc4.cc
@skipline par(

Now, we can assign the parameters in the NED file or from omnetpp.ini.
Assignments in the NED file take precedence. Typically you'll want
to leave most parameter assigments to omnetpp.ini because it makes the model
a lot more flexible.

Here, we assign one parameter in the NED file:

@dontinclude tictoc4.ned
@skip module
@until connections

and the other in omnetpp.ini:

@dontinclude omnetpp.ini
@skipline tictoc4.

Note that because omnetpp.ini supports wildcards, and parameters
assigned from NED files take precedence over the ones in omnetpp.ini,
we could have used

@code
tictoc4.t*c.limit=5
@endcode

or

@code
tictoc4.*.limit=5
@endcode

or even

@code
**.limit=5
@endcode

with the same effect. (The difference between * and ** is that * will not match
a dot and ** will.)

In Tkenv, you can inspect module parameters either in the object tree
on the left-hand side of the main window, or in the Parameters page of
the module inspector (opened via double-clicking on the module icon).

The module with the smaller limit will delete the message and thereby
conclude the simulation.

Sources: @ref tictoc4.ned, @ref txc4.cc, @ref omnetpp.ini


@section s5 Step 5: Modelling processing delay

In the previous models, tic and toc immediately sent back the
received message. Here we'll add some timing: tic and toc will hold the
message for 1 simulated second before sending it back. In @opp
such timing is achieved by the module sending a message to itself.
Such messages are called self-messages (but only because of the way they
are used, otherwise they are ordinary message objects).

We added two cMessage * variables, <tt>event</tt> and <tt>tictocMsg</tt>
to the class, to remember the message we use for timing and message whose
processing delay we are simulating.

@dontinclude txc5.cc
@skip class Txc5
@until public:

We "send" the self-messages with the scheduleAt() function, specifying
when it should be delivered back to the module.

@dontinclude txc5.cc
@skip ::handleMessage
@skipline scheduleAt(

In handleMessage() now we have to differentiate whether a new message
has arrived via the input gate or the self-message came back
(timer expired). Here we are using

@dontinclude txc5.cc
@skipline msg==

but we could have written

@code
    if (msg->isSelfMessage())
@endcode

as well.

We have left out the counter, to keep the source code small.

The result of running the simulation can be seen below.

<img src="step5.gif">

Sources: @ref tictoc5.ned, @ref txc5.cc, @ref omnetpp.ini


@section s6 Step 6: Random numbers and parameters

In this step we'll introduce random numbers. We change the delay from 1s
to a random value which can be set from the NED file or from omnetpp.ini.
Module parameters are able to return random variates; however, to make
use of this feature we have to read the parameter in handleMessage()
every time we use it.

@dontinclude txc6.cc
@skip double delay
@until scheduleAt(

In addition, we'll "lose" (delete) the packet with a small (hardcoded) probability.

@dontinclude txc6.cc
@skip uniform(
@until }

We'll assign the parameters in omnetpp.ini:

@dontinclude omnetpp.ini
@skipline tictoc6.
@skipline tictoc6.

You can try that no matter how many times you re-run the simulation (or
restart it, Simulate|Rebuild network menu item), you'll get exactly the
same results. This is because @opp uses a deterministic algorithm
(by default the Mersenne Twister RNG) to generate random numbers, and
initializes it to the same seed. This is important for reproducible
simulations. You can experiment with different seeds if you add the
following lines to omnetpp.ini:

@code
[General]
seed-0-mt=532569  # or any other 32-bit value
@endcode

From the syntax you have probably guessed that @opp supports
more than one RNGs. That's right, however, all models in this tutorial
use RNG 0.

<i>Exercise: Try other distributions as well.
</i>

Sources: @ref tictoc6.ned, @ref txc6.cc, @ref omnetpp.ini


@section s7 Step 7: Timeout, cancelling timers

In order to get one step closer to modelling networking protocols,
let us transform our model into a stop-and-wait simulation.
This time we'll have separate classes for tic and toc. The basic
scenario is similar to the previous ones: tic and toc will be tossing a
message to one another. However, toc will "lose" the message with some
nonzero probability, and in that case tic will have to resend it.

Here's toc's code:

@dontinclude txc7.cc
@skip Toc7::handleMessage(
@until else

Thanks to the bubble() call in the code, toc'll display a callout whenever
it drops the message.

<img src="step7.gif">

So, tic will start a timer whenever it sends the message. When
the timer expires, we'll assume the message was lost and send another
one. If toc's reply arrives, the timer has to be cancelled.
The timer will be (what else?) a self-message.

@dontinclude txc7.cc
@skip Tic7::handleMessage
@skipline scheduleAt(

Cancelling the timer will be done with the cancelEvent() call. Note that
this does not prevent us from being able to reuse the same
timeout message over and over.

@dontinclude txc7.cc
@skip Tic7::handleMessage
@skipline cancelEvent(

You can read Tic's full source in @ref txc7.cc.

Sources: @ref tictoc7.ned, @ref txc7.cc, @ref omnetpp.ini


@section s8 Step 8: Retransmitting the same message

In this step we refine the previous model.
There we just created another packet if we needed to
retransmit. This is OK because the packet didn't contain much, but
in real life it's usually more practical to keep a copy of the original
packet so that we can re-send it without the need to build it again.

What we do here is keep the original packet and send only copies of it.
We delete the original when toc's acknowledgement arrives.
To make it easier to visually verify the model, we'll include a message
sequence number the message names.

In order to avoid handleMessage() growing too large, we'll put the
corresponding code into two new functions, generateNewMessage()
and sendCopyOf() and call them from handleMessage().

The functions:

@dontinclude txc8.cc
@skip Tic8::generateNewMessage
@until }

@dontinclude txc8.cc
@skip Tic8::sendCopyOf
@until }

Sources: @ref tictoc8.ned, @ref txc8.cc, @ref omnetpp.ini

NEXT: @ref part3
*/

--------------------------------------------------------------------------

/**
@page part3 3. Turning it into a real network

PREV: @ref part2 UP: @ref contents

@section s9 Step 9: More than two nodes

Now we'll make a big step: create several tic modules and connect
them into a network. For now, we'll keep it simple what they do:
one of the nodes generates a message, and the others keep tossing
it around in random directions until it arrives at
a predetermined destination node.

The NED file will need a few changes. First of all, the Txc module will
need to have multiple input and output gates:

@dontinclude tictoc9.ned
@skip simple Txc9
@until endsimple

The [ ] turns the gates into gate vectors. The size of the vector
(the number of gates) will be determined where we use Txc to build
the network.

@skip module Tictoc9
@until endmodule

Here we created 6 modules as a module vector, and connected them.

The resulting topology looks like this:

<img src="step9.gif">

In this version, tic[0] will generate the message to be sent around.
This is done in initialize(), with the help of the index() function which
returns the index of the module in the vector.

The meat of the code is the forwardMessage() function which we invoke
from handleMessage() whenever a message arrives at the node. It draws
a random gate number (size() is the size of the gate vector), and
sends out message on that gate.

@dontinclude txc9.cc
@skip ::forwardMessage
@until }

When the message arrives at tic[3], its handleMessage() will delete the message.

See the full code in @ref txc9.cc.

<i>Exercise: you'll notice that this simple "routing" is not very efficient:
often the packet keeps bouncing between two nodes for a while before it is sent
to a different direction. This can be improved somewhat if nodes don't send
the packet back to the sender. Implement this. Hints: cMessage::arrivalGate(),
cGate::index(). Note that if the message didn't arrive via a gate but was
a self-message, then arrivalGate() returns NULL.
</i>

Sources: @ref tictoc9.ned, @ref txc9.cc, @ref omnetpp.ini


@section s10 Step 10: Defining our message class

In this step the destination address is no longer hardcoded tic[3] -- we draw a
random destination, and we'll add the destination address to the message.

The best way is to subclass cMessage and add destination as a data member.
Hand-coding the message class is usually tedious because it contains
a lot of boilerplate code, so we let @opp generate the class for us.
The message class specification is in tictoc10.msg:

@dontinclude tictoc10.msg
@skip message TicTocMsg10
@until }

The makefile is set up so that the message compiler, opp_msgc is invoked
and it generates tictoc10_m.h and tictoc10_m.cc from the message declaration.
They will contain a generated TicTocMsg10 class subclassed from cMessage;
the class will have getter and setter methods for every field.

We'll include tictoc10_m.h into our C++ code, and we can use TicTocMsg10 as
any other class.

@dontinclude txc10.cc
@skipline tictoc10_m.h

For example, we use the following lines in generateMessage() to create the
message and fill its fields.

@skip ::generateMessage(
@skip TicTocMsg10 *msg
@until return msg

Then, handleMessage() begins like this:

@dontinclude txc10.cc
@skip ::handleMessage(
@until getDestination

In the argument to handleMessage(), we get the message as a cMessage * pointer.
However, we can only access its fields defined in TicTocMsg10 if we cast
msg to TicTocMsg10 *. Plain C-style cast (<code>(TicTocMsg10 *)msg</code>)
is not safe because if the message is <i>not</i> a TicTocMsg10 after all
the program will just crash, causing an error which is difficult to explore.

C++ offers a solution which is called dynamic_cast. Here we use check_and_cast<>()
which is provided by @opp: it tries to cast the pointer via dynamic_cast,
and if it fails it stops the simulation with an error message, similar to the
following:

<img src="step10e.gif">

In the next line, we check if the destination address is the same as the
node's address. The <tt>index()</tt> member function returns the index
of the module in the submodule vector (remember, in the NED file we
declarared it as <tt>tic: Txc10[6]</tt>, so our nodes have addresses 0..5).

To make the model execute longer, after a message arrives to its destination
the destination node will generate another message with a random destination
address, and so forth. Read the full code: @ref txc10.cc.

When you run the model, it'll look like this:

<img src="step10a.gif">

You can double-click on the messages to open an inspector for them.
(You'll either have to temporarily stop the simulation for that,
or to be very fast in handling the mouse). The inspector window
displays lots of useful information; the message fields can be seen
on the Contents page.

<img src="step10b.gif">

Sources: @ref tictoc10.ned, @ref tictoc10.msg, @ref txc10.cc, @ref omnetpp.ini

<i>Exercise: In this model, there is only one message underway at any
given moment: nodes only generate a message when another message arrives
at them. We did it this way to make it easier to follow the simulation.
Change the module class so that instead, it generates messages periodically.
The interval between messages should be a module parameter, returning
exponentially distributed random numbers.
</i>

NEXT: @ref part4
*/

--------------------------------------------------------------------------

/**
@page part4 4. Adding statistics collection

PREV: @ref part3 UP: @ref contents


@section s11 Step 11: Displaying the number of packets sent/received

To get an overview at runtime how many messages each node sent or
received, we've added two counters to the module class: numSent and numReceived.

@dontinclude txc11.cc
@skip class Txc11
@until public:

They are set to zero and WATCH'ed in the initialize() method. Now we
can use the Find/inspect objects dialog (Inspect menu; it is also on
the toolbar) to learn how many packets were sent or received by the
various nodes.

<img src="step11a.gif">

It's true that in this concrete simulation model the numbers will be
roughly the same, so you can only learn from them that intuniform()
works properly. But in real-life simulations it can be very useful that
you can quickly get an overview about the state of various nodes in the
model.

It can be also arranged that this info appears above the module
icons. The <tt>t=</tt> display string tag specifies the text;
we only need to modify the displays string during runtime.
The following code does the job:

@dontinclude txc11.cc
@skip isGUI
@until updateDisplay

@skip ::updateDisplay
@until }

And the result looks like this:

<img src="step11b.gif">

Sources: @ref tictoc11.ned, @ref tictoc11.msg, @ref txc11.cc, @ref omnetpp.ini


@section s12 Step 12: Adding statistics collection

The previous simulation model is does something interesting enough
so that we can collect some statistics. For example, you may be interested
in the average hop count a message has to travel before reaching
its destination.

We'll record in the hop count of every message upon arrival into
an output vector (a sequence of (time,value) pairs, sort of a time series).
We also calculate mean, standard deviation, minimum, maximum values per node, and
write them into a file at the end of the simulation. Then we'll use
off-line tools to analyse the output files.

For that, we add an output vector object (which will record the data into
omnetpp.vec) and a histogram object (which also calculates mean, etc)
to the class.

@dontinclude txc12.cc
@skipline class Txc12
@until public:

When a message arrives at the destination node, we update the statistics.
The following code has been added to handleMessage():

@skip ::handleMessage
@skipline hopCountVector.record
@skipline hopCountStats.collect

hopCountVector.record() call writes the data into omnetpp.vec.
With a large simulation model or long execution time, the omnetpp.vec file
may grow very large. To handle this situation, you can specifically
disable/enable vector in omnetpp.ini, and you can also specify
a simulation time interval in which you're interested
(data recorded outside this interval will be discarded.)

When you begin a new simulation, the existing omnetpp.vec file gets deleted.

Scalar data (collected by the histogram object in this simulation)
have to be recorded manually, in the finish() function.
finish() gets invoked on successful completion of the simulation,
i.e. not when it's stopped with an error. The recordScalar() calls
in the code below write into the omnetpp.sca file.

@skip ::finish
@until }

Unlike omnetpp.vec, omnetpp.sca is <i>not</i> deleted between
simulation runs. Instead, new data are just appended to it. The idea is
that you can collect output from several simulation runs (i.e. with
different input parameters), and analyse them together.

It is possible to use different file names (omnetpp.ini option)
so that e.g. multiple runs write to different files.

You can also view the data during simulation. In the module inspector's
Contents page you'll find the hopCountStats and hopCountVector objects,
and you can open their inspectors (double-click). They will be initially
empty -- run the simulation in Fast (or even Express) mode to get enough
data to be displayed. After a while you'll get something like this:

<img src="step12a.gif">

<img src="step12b.gif">

When you think enough data has been collected, you can stop the simulation
and then we'll analyse the result files (omnetpp.vec and omnetpp.sca) off-line.
You'll need to choose Simulate|Call finish() from the menu
(or click the corresponding toolbar button) before exiting -- this will cause
the finish() functions to run and data to be written into omnetpp.sca.

Sources: @ref tictoc12.ned, @ref tictoc12.msg, @ref txc12.cc, @ref omnetpp.ini

NEXT: @ref part5
*/

--------------------------------------------------------------------------

/**
@page part5 5. Visualizing the results with Plove and Scalars

PREV: @ref part4 UP: @ref contents


@section scalars Scalar statistics

The Scalars tool can be used to visualize the contents of the omnetpp.sca file.
It can draw bar charts, x-y plots (e.g. throughput vs offered load), or
export data via the clipboard for more detailed analysis into spreadsheets
or other programs.

\code
$ scalars omnetpp.sca
\endcode

The program displays the data in a table with columns showing the file name,
run number, module name where it was recorded, and the value. There're
usually too many rows to get an overview, so you can filter by choosing
from (or editing) the three combo boxes at the top. (The filters also
accept the *, ** wildcards.)

<img src="sca1.gif">

You could actually load further scalar files into the window, and thus
analyse them together.

You can copy the selected rows to the clipboard by Edit|Copy or the corresponding
toolbar button, and paste them e.g. into OpenOffice Calc, MS Excel or Gnumeric.

The bar chart toolbar button creates -- well -- a bar chart in a new window.
You can customize the chart by right-clicking on it and choosing from the
context menu. It can also be exported to EPS, GIF, or as metafile via the
Windows clipbard (the latter is not available on Unix of course).

<img src="sca2.gif">


@section plove Plotting the output vectors

Output vector files can be visualized with Plove. Try the following:

\code
$ plove omnetpp.vec
\endcode

The left pane displays vectors that are present in the omnetpp.vec file.
(You can load further vector files as well.)

To plot, you have to copy some vectors to the right pane, select one or
more of them (shift+click and ctrl+click works), and click the Plot icon
on the toolbar.

<img src="plove1.gif">

The graph is displayed in a separate window.

<img src="plove2.gif">

As in Scalars, you can customize the graph and export it in various formats.
For the following screenshot we turned off connecting the data points.

<img src="plove3.gif">

We can apply a filter which plots mean on [0,t). In the main window, right-click
the selected vectors, then choose Pre-plot filtering from the context menu.

<img src="plove4a.gif">

Once in the dialog, choose 'mean' from the filter dropdown list and click OK.
Next time you click the Plot button on the toolbar, you'll get the filtered
charts.

<img src="plove4.gif">

@section conclusion Conclusion

Hope you have found this tutorial a useful introduction into @opp. Comments
and suggestions will be appreciated.

UP: @ref contents
*/

--------------------------------------------------------------------------


/// @page tictoc1.ned tictoc1.ned
/// @include tictoc1.ned

/// @page tictoc2.ned tictoc2.ned
/// @include tictoc2.ned

/// @page tictoc3.ned tictoc3.ned
/// @include tictoc3.ned

/// @page tictoc4.ned tictoc4.ned
/// @include tictoc4.ned

/// @page tictoc5.ned tictoc5.ned
/// @include tictoc5.ned

/// @page tictoc6.ned tictoc6.ned
/// @include tictoc6.ned

/// @page tictoc7.ned tictoc7.ned
/// @include tictoc7.ned

/// @page tictoc8.ned tictoc8.ned
/// @include tictoc8.ned

/// @page tictoc9.ned tictoc9.ned
/// @include tictoc9.ned

/// @page tictoc10.ned tictoc10.ned
/// @include tictoc10.ned

/// @page tictoc11.ned tictoc11.ned
/// @include tictoc11.ned

/// @page tictoc12.ned tictoc12.ned
/// @include tictoc12.ned

/// @page txc1.cc txc1.cc
/// @include txc1.cc

/// @page txc2.cc txc2.cc
/// @include txc2.cc

/// @page txc3.cc txc3.cc
/// @include txc3.cc

/// @page txc4.cc txc4.cc
/// @include txc4.cc

/// @page txc5.cc txc5.cc
/// @include txc5.cc

/// @page txc6.cc txc6.cc
/// @include txc6.cc

/// @page txc7.cc txc7.cc
/// @include txc7.cc

/// @page txc8.cc txc8.cc
/// @include txc8.cc

/// @page txc9.cc txc9.cc
/// @include txc9.cc

/// @page txc10.cc txc10.cc
/// @include txc10.cc

/// @page txc11.cc txc11.cc
/// @include txc11.cc

/// @page txc12.cc txc12.cc
/// @include txc12.cc

/// @page tictoc10.msg tictoc10.msg
/// @include tictoc10.msg

/// @page tictoc11.msg tictoc11.msg
/// @include tictoc11.msg

/// @page tictoc12.msg tictoc12.msg
/// @include tictoc12.msg

/// @page omnetpp.ini omnetpp.ini
/// @include omnetpp.ini


