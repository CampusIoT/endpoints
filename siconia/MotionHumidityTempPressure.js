/**
* Simple program for Sagemcom Siconia : send sensors values periodically and send motion sensor value when a motion has been detected.
* Sagemcom library http://193.253.237.167:380/loradevice-doc/javadoc.html
* Espruino library https://www.espruino.com/Reference#software
**/
var MOTION_THRESHOLD = 200; //0.2G
var INTERVAL = 10*60*1000; /* interval used to send sensors values */
var DUTYCYCLE = false;

const MOTION_PORT=8;
const REPORT_PORT=1;
const BUTTON_PORT=7;
const PARAMS_SET_PORT=2; // TODO
const RTC_GET_PORT=3; // TODO
const RTC_SET_PORT=4; // TODO
const REJOIN_PORT=5; // TODO

function sendMotion() {
	// check timeOff
	//if(L.getTimeOff() > 0) return;

	var payload = new ArrayBuffer(12);
	var m = M.read();
	var motion = new Float32Array(payload, 0);
	motion[0]= m[0];
	motion[1]= m[1];
	motion[2]= m[2];
	L.send(MOTION_PORT, payload);
}

function _sendAll(event) {
	// check timeOff
	//if(L.getTimeOff() > 0) return;

	var payload = new ArrayBuffer(25);
	var i8=new Int8Array(payload);
	var data=new Float32Array(payload);

	var m=M.read();
	data[0]=m[0];
	data[1]=m[1];
	data[2]=m[2];

	// TODO optimize T size (8 bit)
	T.init();
	wait(50);
	data[3]=T.read();
	T.deinit();

	// TODO optimize Hu size (8 bit)
	Hu.init();
	wait(50);
	data[4]=Hu.read();
	Hu.deinit();

	// TODO optimize Pr size (8 bit)
	Pr.init();
	wait(50);
	data[5]=Pr.read();
	Pr.deinit();

	// battery level
	i8[24]=Board.battery().percent;

	L.send(event,payload);
}

function sendAll() {
	_sendAll(REPORT_PORT)
}

/* Button :
	- Appui simple court   -> Cu activ� ou non
	- Appui long [>6s] 	   -> Activer ou desactiver le sc�nario
*/
function onReleaseEvent(delay) {
  if (delay>6000) {
    if (started == false) {
			startup();
		} else {
			L.airplane(true);
			led.blink(LED2,3);
			started = false;
			hwreset();
		}
  } else {
		_sendAll(BUTTON_PORT);
	}
}
B.on('release', onReleaseEvent);
//B.on('onPress', onPressEvent);


var intervalID;

var started = false;

function stop() {
    L.dutycycle(true);
    L.airplane(true);
		started = false;
}

function onJoin() {
    green.blink(3);
    //print("node has joined devAddr="+L.devAddr());

		M.init();
		M.motion(MOTION_THRESHOLD, sendMotion);
		intervalID = setInterval(sendAll, INTERVAL);

}

function onTx(tx_status) {
	/*
	Passed parameters : 'tx_status'

	0 : Service performed successfully

	1 : An error occured during the execution of the service

	2 : Device is in airplane mode

	3->10 : Reserved for reception errors

	11 : LoraMac busy executing previous command

	12 : A join procedure is ongoing and not yet accepted

	13 : Payload size exceeds the Datarate maximum allowed size

	14 : MAC command error

	15 : No free channel for transmission
	*/
    orange.blink();
		//print("tx tx_status="+tx_status);

}

function onEndTx(tx_status) {
    green.blink();
		/*
		Passed parameters : 'tx_status'

0 : Service performed successfully

1 : An error occured during the execution of the service

2 : Device is in airplane mode

3->10: Reserved for reception errors

11 : LoraMac busy executing previous command

12 : A join procedure is ongoing and not yet accepted

13 : Payload size exceeds the Datarate maximum allowed size

14 : MAC command error

15 : No free channel for transmission
		*/
		//print("endTx tx_status="+tx_status);
}

function onRx(parameters) {
/**
	Passed parameters : 'rx_status' ->

	0 : Service performed successfully

	1 : An error occured during the execution of the service

	2 : Device is in airplane mode

	3 : An Rx timeout occured on receive window

	4 : An Rx error occured on receive window

	5 : A frame with an invalid downlink counter was received

	> : Other error

	'rx_port' -> Rx Port

	'data' -> An array containing the received payload

	'rssi' -> Received signal strength of the frame

	'snr' -> Signal noise ratio of the received frame

	'window' -> First window or second window
*/

    green.blink(3);
		//print("rx parameters="+parameters);
}

// LoRaWAN Events section
// The ‘join’ event is called when the device join the network.
L.on('join', onJoin);

// The ‘tx’ event is called when a frame has been sent.
L.on('tx', onTx);

// The ‘endtx’ event is called once the complete transmission procedure (including an eventual reception) is finished.
// L.on('‘endtx’', onEndTx);

// The ‘rx’ event is called when a frame has been received.
L.on('rx', onRx);


// Passed parameters : 'size' -> Size of the MAC reply in bytes
//L.on('mac', function() { ... });

/* Startup */
function startup() {
		red.blink(3);
    L.dutycycle(DUTYCYCLE);
    L.airplane(false);
		started = true;
		L.join();
}
Board.on('init', startup);
