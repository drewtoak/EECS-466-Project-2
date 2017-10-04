========================================================================
EECS 466: Assignment 4
authors: Andrew Hwang and Tim Kuo
========================================================================

How to execute application:

	1.	Open solution file HwangKuoAS4.sln in Visual Studio.
	2.	Build and Run the Local Windows Debugger.

Application functions:

	1.	LEFT mouse button - swings the viewing axis based on the mouse motion. 
	2.	MIDDLE mouse button - moves the camera up, down, left, and right in the plane perpendicular to its viewing axis. 
	3.	RIGHT mouse button - moves the camera forward and back along the viewing vector.
	Keyboard Commands :
	NOTE : the following are with respect to the WORLD coordinate system
	4.	"4" : negative translation along x axis
	5.	"6" : positive translation along x axis
	6.	"8" : positive translation along y axis
	7.	"2" : negative translation along y axis
	8.	"9" : positive translation along z axis
	9.	"1" : negative translation along z axis
	10.	"[" : negative rotation around x axis
	11.	"]" : positive rotation around x axis
	12.	";" : negative rotation around y axis
	13.	"'" : positive rotation around y axis
	14.	"." : negative rotation around z axis
	15.	"/" : positive rotation around z axis
	16.	"=" : increase uniform scaling
	17.	"-" : decrease uniform scaling
	NOTE : the following are with respect to the LOCAL object coordinate system
	18.	"i" : negative rotation around local x axis
	19.	"o" : positive rotation around local x axis
	20.	"k" : negative rotation around local y axis
	21.	"l" : positive rotation around local y axis
	22.	"m" : negative rotation around local z axis
	23.	"," : positive rotation around local z axis
	Other keyboard commands :
	24.	"a" : toggle display of coordinate axes, should display world and local object coordinate axes
	25.	"c" : snap camera to pointing at the world origin and pointing at the object at alternating order
	26.	"p" : toggle between perspective and orthogonal projection (already in the template)
	27.	"q" : exit program