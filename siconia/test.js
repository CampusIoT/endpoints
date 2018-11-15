/**
* Name         : test
*
**/
var scriptVersion = "1.1";
var started = false;
var Thshld = 200; //mg 1.96 m/s2

function stop() {
	L.dutycycle(true);
	L.airplane(true);
}


/* Startup */
function startup() {
	L.dutycycle(false);
	L.airplane(false);
	started = false;
}
Board.on('init', startup);

L.devEui()
L.appEui()

[ 72, 131, 199, 223, 48, 6, 18, 62 ]
[ 72, 131, 199, 223, 48, 6, 0, 0 ]


var devEui = []
var appEui = []
L.devEui().forEach(function(e) { devEui.push(e.toString(16));});
L.appEui().forEach(function(e) { appEui.push(e.toString(16));});
devEui
appEui


function onJoin() {
	green.blink(3);
	print("node has joined devAddr="+L.devAddr())
}
L.on('join', onJoin);

L.join()
L.isJoining()
L.isJoined()
L.getTimeOff()
L.payloadLimit()
L.adr()
L.getDR()

L.on('tx', 'orange.blink()')
L.send(22,"Hello")
L.getTimeOff()
L.getDR()

L.dutycycle(true);
L.send(22,"World")
L.getTimeOff()
L.getDR()

L.dutycycle(false);

function confirmFunction() {
print("message confirmed");
}
// confirm 3 times
L.confirm(4, confirmFunction)
//L.confirm(0, confirmFunction)

L.sendConf(3,44,"Hello world")
L.getTimeOff()
L.getDR()


function onLCAns(margin, nbgw) {
	print("margin="+margin+", nbgw="+nbgw);
}
L.on("lc",onLCAns)
L.lc(1)

L.lc(1)


// LED
red.blink(5)
orange.blink()

green.delay = 2000
green.blink(3)

// Button
B.state()

B.on('short', 'green.blink()')
B.on('long', 'red.blink()')

function onPress() {
print("button pressed");
red.blink();
}
function onReleased(delayMs) {
print("button released : "+delayMs+"ms");
}

B.on("press", onPress);
B.on("release", onReleased);


// Timer
function onTime() {
	print("timer is running");
}
var timerId  = setInterval(onTime, 3000)

clearInterval(timerId)

// Temperature
T.init()
print("temperature="+T.read()+"C")
T.deinit()


// Humidity
Hu.init()
print("humidity="+Hu.read()+"%")
Hu.readRaw()
Hu.deinit()

// Barometer:
Pr.init()
print("pressure="+Pr.read()/100+"hPa")
Pr.readRaw()
Pr.deinit()

// Motion
// M.init()
//It is possible to set an alternative configuration if needed. Below, the possible values for each parameter:
// - Rate : [ 1.6, 12.5, 26, 52, 104, 208, 416, 833, 1660, 3330, 6660 ] expressed in Hz
// - Scale : [ 2, 4, 8, 16 ] expressed in g
// To set, the new configuration, call the initialization method with the chosen configuration
M.init(2,104)

function onMotion(axis) {
	print("Motion threshold reached in axis="+ axis);
}
M.motion(1000, onMotion);

// If the accelerometer is not needed anymore, call the de-initialization method in order to shut down the peripheral and thus, reduce power consumption.
M.deinit()


function onTime() {
	print("timer is running");
}
var timerId  = setInterval(onTime, 3000)

clearInterval(timerId)



function sendSensorCurrentValues() {

	var payload = new ArrayBuffer(24);
	var data=new Float32Array(payload, 0);

	var m=M.read();
	data[0]=m[0];
	data[1]=m[1];
	data[2]=m[2];

	T.init();
	wait(50);
	data[3]=T.read();
	T.deinit();

	Hu.init();
	wait(50);
	data[4]=Hu.read();
	Hu.deinit();

	Pr.init();
	wait(50);
	data[5]=Pr.read();
	Pr.deinit();
	print("send payload="+payload);
	L.send(1,payload);
}

var period = 5*60*1000;
var timerId  = setInterval(sendSensorCurrentValues, period);
print("sent timer period="+period+" id="+ timerId);


function deinitAll() {
	T.deinit();
	Hu.deinit();
	Pr.deinit();
	M.deinit();
}
deinitAll();


process.memory()
swVersion
hwVersion

hwreset()

stop()
